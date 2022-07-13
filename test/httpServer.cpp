#include "httpServer.hpp"
#include <chrono>
#include <string>
#include <system_error>
#include <thread>

namespace cpr {
std::string HttpServer::GetBaseUrl() {
    return "http://127.0.0.1:" + std::to_string(GetPort());
}

uint16_t HttpServer::GetPort() {
    // Unassigned port number in the ephemeral range
    return 61936;
}

mg_connection* HttpServer::initServer(mg_mgr* mgr, mg_event_handler_t event_handler) {
    // Based on: https://mongoose.ws/tutorials/http-server/
    mg_mgr_init(mgr);
    std::string port = std::to_string(GetPort());
    mg_connection* c = mg_http_listen(mgr, GetBaseUrl().c_str(), event_handler, this);
    if (!c) {
        throw std::system_error(errno, std::system_category(), "Failed to listen at port " + port);
    }
    return c;
}

void HttpServer::acceptConnection(mg_connection* /* conn */) {}

void HttpServer::OnRequestHello(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} == std::string{"OPTIONS"}) {
        OnRequestOptions(conn, msg);
    } else {
        std::string response{"Hello world!"};
        std::string headers = "Content-Type: text/html\r\n";
        mg_http_reply(conn, 200, headers.c_str(), response.c_str());
    }
}

void HttpServer::OnRequestRoot(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} == std::string{"OPTIONS"}) {
        OnRequestOptions(conn, msg);
    } else {
        std::string errorMessage{"Method Not Allowed"};
        SendError(conn, 405, errorMessage);
    }
}

void HttpServer::OnRequestNotFound(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} == std::string{"OPTIONS"}) {
        OnRequestOptions(conn, msg);
    } else {
        std::string errorMessage{"Not Found"};
        SendError(conn, 404, errorMessage);
    }
}

void HttpServer::OnRequestOptions(mg_connection* conn, mg_http_message* /*msg*/) {
    std::string headers =
            "Content-Type: text/plain\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Credentials: true\r\n"
            "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, PATCH, OPTIONS\r\n"
            "Access-Control-Max-Age: 3600\r\n";

    std::string response;
    mg_http_reply(conn, 200, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestTimeout(mg_connection* conn, mg_http_message* msg) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    OnRequestHello(conn, msg);
}

// Send the header, then send "Hello world!" every 100ms
// For this, we use a mongoose timer
void HttpServer::OnRequestLowSpeedTimeout(mg_connection* conn, mg_http_message* /* msg */, mg_mgr* mgr) {
    std::string response{"Hello world!"};
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", response.length() * 20);

    struct timer_fn_arg {
        mg_mgr* mgr;
        mg_connection* connection;
        mg_timer timer;
    };
    timer_fn_arg* timer_arg = new timer_fn_arg{mgr, conn, mg_timer {}};
    mg_timer_init(
            &mgr->timers, &timer_arg->timer, 100, MG_TIMER_REPEAT,
            // The following lambda function gets executed each time the timer is called.
            // It sends "Hello world!" to the client each 100ms at most 20 times.
            [](void* arg) {
                static int counter{0};
                std::string response{"Hello world!"};
                auto* timer_arg = static_cast<timer_fn_arg*>(arg);
                mg_send(timer_arg->connection, response.c_str(), response.length());
                std::cout << counter << '\n';
                // If we reached the 20th iteration or if the connection is not active anymore, we remove the timer
                if (++counter == 20 || IsConnectionActive(timer_arg->mgr, timer_arg->connection)) {
                    std::cout << "Finished" << std::endl;
                    mg_timer_free(&timer_arg->mgr->timers, &timer_arg->timer);
                    delete timer_arg;
                }
            },
            timer_arg);
}

// Before and after calling an endpoint that calls this method, the test needs to wait until all previous connections are closed
// The nested call to mg_mgr_poll can lead to problems otherwise
void HttpServer::OnRequestLowSpeed(mg_connection* conn, mg_http_message* /*msg*/, mg_mgr* mgr) {
    std::string response{"Hello world!"};
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", response.length());
    mg_timer_add(
            mgr, 2000, MG_TIMER_ONCE,
            [](void* connection) {
                std::string response{"Hello world!"};
                mg_send(static_cast<mg_connection*>(connection), response.c_str(), response.length());
            },
            conn);
}

// Before and after calling an endpoint that calls this method, the test needs to wait until all previous connections are closed
// The nested call to mg_mgr_poll can lead to problems otherwise
void HttpServer::OnRequestLowSpeedBytes(mg_connection* conn, mg_http_message* /*msg*/, mg_mgr* mgr) {
    std::string response{'a'};
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", response.length() * 20);
    mg_mgr_poll(mgr, 0);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    for (size_t i = 0; i < 20 && !conn->is_closing; ++i) {
        mg_send(conn, response.c_str(), response.length());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        mg_mgr_poll(mgr, 0);
    }
}

void HttpServer::OnRequestBasicCookies(mg_connection* conn, mg_http_message* /*msg*/) {
    time_t t = time(nullptr) + 5; // Valid for 1 hour
    char expire[100], expire_epoch[100];
    snprintf(expire_epoch, sizeof(expire_epoch), "%lu", static_cast<unsigned long>(t));
    strftime(expire, sizeof(expire), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
    std::string cookie{"cookie=chocolate; expires=\"" + std::string{expire} + "\"; http-only;"};
    std::string cookie2{"icecream=vanilla; expires=\"" + std::string{expire} + "\"; http-only;"};
    std::string headers =
            "Content-Type: text/html\r\n"
            "Set-Cookie: " +
            cookie +
            "\r\n"
            "Set-Cookie: " +
            cookie2 + "\r\n";
    std::string response{"Hello world!"};
    mg_http_reply(conn, 200, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestEmptyCookies(mg_connection* conn, mg_http_message* /*msg*/) {
    time_t t = time(nullptr) + 5; // Valid for 1 hour
    char expire[100];
    char expire_epoch[100];
    snprintf(expire_epoch, sizeof(expire_epoch), "%lu", static_cast<unsigned long>(t));
    strftime(expire, sizeof(expire), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
    std::string cookie{"cookie=; expires=\"" + std::string{expire} + "\"; http-only;"};
    std::string cookie2{"icecream=; expires=\"" + std::string{expire} + "\"; http-only;"};
    std::string headers =
            "Content-Type: text/html\r\n"
            "Set-Cookie: " +
            cookie +
            "\r\n"
            "Set-Cookie: " +
            cookie2 + "\r\n";
    std::string response{"Hello world!"};
    mg_http_reply(conn, 200, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestCheckCookies(mg_connection* conn, mg_http_message* msg) {
    mg_str* request_cookies;
    if ((request_cookies = mg_http_get_header(msg, "Cookie")) == nullptr) {
        std::string errorMessage{"Cookie not found"};
        SendError(conn, 400, errorMessage);
        return;
    }
    std::string cookie_str{request_cookies->ptr, request_cookies->len};

    if (cookie_str.find("cookie=chocolate;") == cookie_str.npos || cookie_str.find("icecream=vanilla;") == cookie_str.npos) {
        std::string errorMessage{"Cookies not found"};
        SendError(conn, 400, errorMessage);
    }

    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestV1Cookies(mg_connection* conn, mg_http_message* /*msg*/) {
    time_t t = time(nullptr) + 5; // Valid for 1 hour
    char expire[100], expire_epoch[100];
    snprintf(expire_epoch, sizeof(expire_epoch), "%lu", static_cast<unsigned long>(t));
    strftime(expire, sizeof(expire), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
    std::string v1cookie{"cookie=\"value with spaces (v1 cookie)\"; expires=\"" + std::string{expire} + "\"; http-only;"};

    std::string headers =
            "Content-Type: text/html\r\n"
            "Set-Cookie: " +
            v1cookie + "\r\n";
    std::string response{"Hello world!"};
    mg_http_reply(conn, 200, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestCheckV1Cookies(mg_connection* conn, mg_http_message* msg) {
    mg_str* request_cookies;
    if ((request_cookies = mg_http_get_header(msg, "Cookie")) == nullptr) {
        std::string errorMessage{"Cookie not found"};
        SendError(conn, 400, errorMessage);
        return;
    }
    std::string cookie_str{request_cookies->ptr, request_cookies->len};

    if (cookie_str.find("cookie=\"value with spaces (v1 cookie)\";") == std::string::npos) {
        std::string errorMessage{"Cookie with space not found"};
        SendError(conn, 400, errorMessage);
        return;
    }

    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestBasicAuth(mg_connection* conn, mg_http_message* msg) {
    mg_str* requested_auth;
    std::string auth{"Basic"};
    if ((requested_auth = mg_http_get_header(msg, "Authorization")) == nullptr || mg_ncasecmp(requested_auth->ptr, auth.c_str(), auth.length()) != 0) {
        std::string errorMessage{"Unauthorized"};
        SendError(conn, 401, errorMessage);
        return;
    }
    std::string auth_string{requested_auth->ptr, requested_auth->len};
    size_t basic_token = auth_string.find(' ') + 1;
    auth_string = auth_string.substr(basic_token, auth_string.length() - basic_token);
    auth_string = Base64Decode(auth_string);
    size_t colon = auth_string.find(':');
    std::string username = auth_string.substr(0, colon);
    std::string password = auth_string.substr(colon + 1, auth_string.length() - colon - 1);
    if (username == "user" && password == "password") {
        OnRequestHeaderReflect(conn, msg);
    } else {
        std::string errorMessage{"Unauthorized"};
        SendError(conn, 401, errorMessage);
    }
}

void HttpServer::OnRequestBearerAuth(mg_connection* conn, mg_http_message* msg) {
    mg_str* requested_auth;
    std::string auth{"Bearer"};
    if ((requested_auth = mg_http_get_header(msg, "Authorization")) == nullptr || mg_ncasecmp(requested_auth->ptr, auth.c_str(), auth.length()) != 0) {
        std::string errorMessage{"Unauthorized"};
        SendError(conn, 401, errorMessage);
        return;
    }
    std::string auth_string{requested_auth->ptr, requested_auth->len};
    size_t basic_token = auth_string.find(' ') + 1;
    auth_string = auth_string.substr(basic_token, auth_string.length() - basic_token);
    if (auth_string == "the_token") {
        OnRequestHeaderReflect(conn, msg);
    } else {
        std::string errorMessage{"Unauthorized"};
        SendError(conn, 401, errorMessage);
    }
}

void HttpServer::OnRequestBasicJson(mg_connection* conn, mg_http_message* /*msg*/) {
    std::string response =
            "[\n"
            "  {\n"
            "    \"first_key\": \"first_value\",\n"
            "    \"second_key\": \"second_value\"\n"
            "  }\n"
            "]";
    std::string headers = "Content-Type: application/json\r\n";
    mg_http_reply(conn, 200, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestHeaderReflect(mg_connection* conn, mg_http_message* msg) {
    std::string response = "Header reflect " + std::string{msg->method.ptr, msg->method.len};
    std::string headers;
    bool hasContentTypeHeader = false;
    for (auto& header : msg->headers) {
        if (!header.name.ptr) {
            continue;
        }

        std::string name = std::string(header.name.ptr, header.name.len);
        if (std::string{"Content-Type"} == name) {
            hasContentTypeHeader = true;
        }

        if (std::string{"Host"} != name && std::string{"Accept"} != name) {
            if (header.value.ptr) {
                headers.append(name + ": " + std::string(header.value.ptr, header.value.len) + "\r\n");
            }
        }
    }

    if (!hasContentTypeHeader) {
        headers.append("Content-Type: text/html\r\n");
    }
    mg_http_reply(conn, 200, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestTempRedirect(mg_connection* conn, mg_http_message* msg) {
    // Get the requested target location:
    std::string location;
    for (auto& header : msg->headers) {
        if (!header.name.ptr) {
            continue;
        }

        std::string name = std::string(header.name.ptr, header.name.len);
        if (std::string{"RedirectLocation"} == name) {
            location = std::string(header.value.ptr, header.value.len);
            break;
        }
    }

    // Check if the request contains a valid location, else default to 'hello.html':
    if (location.empty()) {
        location = "hello.html";
    }
    std::string headers = "Location: " + location + "\r\n";
    std::string response = "Moved Temporarily";
    mg_http_reply(conn, 302, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestPermRedirect(mg_connection* conn, mg_http_message* msg) {
    // Get the requested target location:
    std::string location;
    for (auto& header : msg->headers) {
        if (!header.name.ptr) {
            continue;
        }

        std::string name = std::string(header.name.ptr, header.name.len);
        if (std::string{"RedirectLocation"} == name) {
            location = std::string(header.value.ptr, header.value.len);
            break;
        }
    }

    // Check if the request contains a valid location, else default to 'hello.html':
    if (location.empty()) {
        location = "hello.html";
    }
    std::string headers = "Location: " + location + "\r\n";
    std::string response = "Moved Permanently";

    mg_http_reply(conn, 301, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestTwoRedirects(mg_connection* conn, mg_http_message* /*msg*/) {
    std::string response = "Moved Permanently";
    std::string headers = "Location: permanent_redirect.html\r\n";
    mg_http_reply(conn, 301, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestUrlPost(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} != std::string{"POST"}) {
        std::string errorMessage{"Method Not Allowed"};
        SendError(conn, 405, errorMessage);
        return;
    }

    std::string headers = "Content-Type: application/json\r\n";

    char x[100];
    char y[100];
    mg_http_get_var(&(msg->body), "x", x, sizeof(x));
    mg_http_get_var(&(msg->body), "y", y, sizeof(y));
    std::string x_string{x};
    std::string y_string{y};
    std::string response;
    if (y_string.empty()) {
        response = std::string{
                "{\n"
                "  \"x\": " +
                x_string +
                "\n"
                "}"};
    } else {
        response = std::string{
                "{\n"
                "  \"x\": " +
                x_string +
                ",\n"
                "  \"y\": " +
                y_string +
                ",\n"
                "  \"sum\": " +
                std::to_string(atoi(x) + atoi(y)) +
                "\n"
                "}"};
    }
    mg_http_reply(conn, 201, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestBodyGet(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} != std::string{"GET"}) {
        std::string errorMessage{"Method Not Allowed"};
        SendError(conn, 405, errorMessage);
        return;
    }
    std::array<char, 100> message{};
    mg_http_get_var(&(msg->body), "message", message.data(), message.size());
    if (msg->body.len <= 0) {
        std::string errorMessage{"No Content"};
        SendError(conn, 405, errorMessage);
        return;
    }
    std::string response = message.data();
    std::string headers = "Content-Type: text/html\r\n";
    mg_http_reply(conn, 200, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestJsonPost(mg_connection* conn, mg_http_message* msg) {
    mg_str* content_type{nullptr};
    if ((content_type = mg_http_get_header(msg, "Content-Type")) == nullptr || std::string{content_type->ptr, content_type->len} != "application/json") {
        std::string errorMessage{"Unsupported Media Type"};
        SendError(conn, 415, errorMessage);
        return;
    }

    std::string headers = "Content-Type: application/json\r\n";
    mg_http_reply(conn, 201, headers.c_str(), msg->body.ptr);
}

void HttpServer::OnRequestFormPost(mg_connection* conn, mg_http_message* msg) {
    size_t pos{0};
    std::map<std::string, std::string> forms;
    std::map<std::string, std::string> filenames;
    struct mg_http_part part {};

    while ((pos = mg_http_next_multipart(msg->body, pos, &part)) > 0) {
        forms[std::string(part.name.ptr, part.name.len)] = std::string(part.body.ptr, part.body.len);
        filenames[std::string(part.name.ptr, part.name.len)] = std::string(part.filename.ptr, part.filename.len);
    }

    std::string x = forms["x"];

    std::string headers = "Content-Type: application/json\r\n";
    std::string response;
    if (forms.find("y") == forms.end()) {
        if (!filenames["x"].empty()) {
            response = std::string{
                    "{\n"
                    "  \"x\": " +
                    filenames["x"] + "=" + forms["x"] +
                    "\n"
                    "}"};
        } else {
            response = std::string{
                    "{\n"
                    "  \"x\": " +
                    forms["x"] +
                    "\n"
                    "}"};
        }
    } else {
        response = std::string{
                "{\n"
                "  \"x\": " +
                forms["x"] +
                ",\n"
                "  \"y\": " +
                forms["y"] +
                ",\n"
                "  \"sum\": " +
                std::to_string(atoi(forms["x"].c_str()) + atoi(forms["y"].c_str())) +
                "\n"
                "}"};
    }
    mg_http_reply(conn, 201, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestDelete(mg_connection* conn, mg_http_message* msg) {
    bool has_json_header = false;
    for (auto& header : msg->headers) {
        if (!header.name.ptr) {
            continue;
        }

        std::string name = std::string(header.name.ptr, header.name.len);
        std::string value = std::string(header.value.ptr, header.value.len);
        if (std::string{"Content-Type"} == name && std::string{"application/json"} == value) {
            has_json_header = true;
            break;
        }
    }
    if (std::string{msg->method.ptr, msg->method.len} == std::string{"DELETE"}) {
        std::string headers;
        std::string response = "Patch success";
        if (!has_json_header) {
            headers = "Content-Type: text/html\r\n";
            response = "Delete success";
        } else {
            headers = "Content-Type: application/json\r\n";
            response = std::string{msg->body.ptr, msg->body.len};
        }
        mg_http_reply(conn, 200, headers.c_str(), response.c_str());
    } else {
        std::string errorMessage{"Method Not Allowed"};
        SendError(conn, 405, errorMessage);
    }
}

void HttpServer::OnRequestDeleteNotAllowed(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} == std::string{"DELETE"}) {
        std::string errorMessage{"Method Not Allowed"};
        SendError(conn, 405, errorMessage);
    } else {
        std::string headers = "Content-Type: text/html\r\n";
        std::string response = "Delete success";
        mg_http_reply(conn, 200, headers.c_str(), response.c_str());
    }
}

void HttpServer::OnRequestPut(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} == std::string{"PUT"}) {
        char x[100];
        char y[100];
        mg_http_get_var(&(msg->body), "x", x, sizeof(x));
        mg_http_get_var(&(msg->body), "y", y, sizeof(y));
        std::string x_string = std::string{x};
        std::string y_string = std::string{y};
        std::string headers = "Content-Type: application/json\r\n";
        std::string response;
        if (y_string.empty()) {
            response = std::string{
                    "{\n"
                    "  \"x\": " +
                    x_string +
                    "\n"
                    "}"};
        } else {
            response = std::string{
                    "{\n"
                    "  \"x\": " +
                    x_string +
                    ",\n"
                    "  \"y\": " +
                    y_string +
                    ",\n"
                    "  \"sum\": " +
                    std::to_string(atoi(x) + atoi(y)) +
                    "\n"
                    "}"};
        }
        mg_http_reply(conn, 200, headers.c_str(), response.c_str());
    } else {
        std::string errorMessage{"Method Not Allowed"};
        SendError(conn, 405, errorMessage);
    }
}

void HttpServer::OnRequestReflectPost(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} != std::string{"POST"}) {
        std::string errorMessage{"Method Not Allowed"};
        SendError(conn, 405, errorMessage);
    }

    std::string response = std::string{msg->body.ptr, msg->body.len};
    std::string headers;
    for (auto& header : msg->headers) {
        if (!header.name.ptr) {
            continue;
        }

        std::string name{header.name.ptr, header.name.len};
        if (std::string{"Host"} != name && std::string{"Accept"} != name) {
            if (header.value.ptr) {
                headers.append(name + ": " + std::string(header.value.ptr, header.value.len) + "\r\n");
            }
        }
    }
    mg_http_reply(conn, 200, headers.c_str(), response.c_str());
}

void HttpServer::OnRequestPutNotAllowed(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} == std::string{"PUT"}) {
        std::string errorMessage{"Method Not Allowed"};
        SendError(conn, 405, errorMessage);
    } else {
        std::string headers = "Content-Type: text/html\r\n";
        std::string response = "Delete success";
        mg_http_reply(conn, 200, headers.c_str(), response.c_str());
    }
}

void HttpServer::OnRequestPatch(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} == std::string{"PATCH"}) {
        char x[100];
        char y[100];
        mg_http_get_var(&(msg->body), "x", x, sizeof(x));
        mg_http_get_var(&(msg->body), "y", y, sizeof(y));
        std::string x_string = std::string{x};
        std::string y_string = std::string{y};
        std::string headers = "Content-Type: application/json\r\n";
        std::string response;
        if (y_string.empty()) {
            response = std::string{
                    "{\n"
                    "  \"x\": " +
                    x_string +
                    "\n"
                    "}"};
        } else {
            response = std::string{
                    "{\n"
                    "  \"x\": " +
                    x_string +
                    ",\n"
                    "  \"y\": " +
                    y_string +
                    ",\n"
                    "  \"sum\": " +
                    std::to_string(atoi(x) + atoi(y)) +
                    "\n"
                    "}"};
        }
        mg_http_reply(conn, 200, headers.c_str(), response.c_str());
    } else {
        std::string errorMessage{"Method Not Allowed"};
        SendError(conn, 405, errorMessage);
    }
}

void HttpServer::OnRequestPatchNotAllowed(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} == std::string{"PATCH"}) {
        std::string errorMessage{"Method Not Allowed"};
        SendError(conn, 405, errorMessage);
    } else {
        std::string headers = "Content-Type: text/html\r\n";
        std::string response = "Delete success";
        mg_http_reply(conn, 200, headers.c_str(), response.c_str());
    }
}

void HttpServer::OnRequestDownloadGzip(mg_connection* conn, mg_http_message* msg) {
    if (std::string{msg->method.ptr, msg->method.len} == std::string{"DOWNLOAD"}) {
        std::string errorMessage{"Method Not Allowed"};
        SendError(conn, 405, errorMessage);
    } else {
        std::string encoding;
        std::string range;
        std::vector<std::pair<int64_t, int64_t>> ranges;

        for (auto& header : msg->headers) {
            if (!header.name.ptr) {
                continue;
            }

            std::string name = std::string(header.name.ptr, header.name.len);
            if (std::string{"Accept-Encoding"} == name) {
                encoding = std::string(header.value.ptr, header.value.len);
            } else if (std::string{"Range"} == name) {
                range = std::string(header.value.ptr, header.value.len);
            }
        }
        if (encoding.find("gzip") == std::string::npos) {
            std::string errorMessage{"Invalid encoding: " + encoding};
            SendError(conn, 405, errorMessage);
            return;
        }
        if (!range.empty()) {
            std::string::size_type eq_pos = range.find('=');
            if (eq_pos == std::string::npos) {
                std::string errorMessage{"Invalid range header: " + range};
                SendError(conn, 405, errorMessage);
                return;
            }

            int64_t current_start_index = eq_pos + 1;
            int64_t current_end_index;
            std::string::size_type range_len = range.length();
            std::string::size_type com_pos;
            std::string::size_type sep_pos;
            bool more_ranges_exists;

            do {
                com_pos = range.find(',', current_start_index);
                if (com_pos < range_len) {
                    current_end_index = com_pos - 1;
                } else {
                    current_end_index = range_len - 1;
                }

                std::pair<int64_t, int64_t> current_range{0, -1};

                sep_pos = range.find('-', current_start_index);
                if (sep_pos == std::string::npos) {
                    std::string errorMessage{"Invalid range format " + range.substr(current_start_index, current_end_index)};
                    SendError(conn, 405, errorMessage);
                    return;
                }
                if (sep_pos == eq_pos + 1) {
                    std::string errorMessage{"Suffix ranage not supported: " + range.substr(current_start_index, current_end_index)};
                    SendError(conn, 405, errorMessage);
                    return;
                }

                current_range.first = std::strtoll(range.substr(current_start_index, sep_pos - 1).c_str(), nullptr, 10);
                if (current_range.first == LLONG_MAX || current_range.first == LLONG_MIN) {
                    std::string errorMessage{"Start range is invalid number: " + range.substr(current_start_index, current_end_index)};
                    SendError(conn, 405, errorMessage);
                    return;
                }

                std::string er_str = range.substr(sep_pos + 1, current_end_index);
                if (!er_str.empty()) {
                    current_range.second = std::strtoll(er_str.c_str(), nullptr, 10);
                    if (current_range.second == 0 || current_range.second == LLONG_MAX || current_range.second == LLONG_MIN) {
                        std::string errorMessage{"End range is invalid number: " + range.substr(current_start_index, current_end_index)};
                        SendError(conn, 405, errorMessage);
                        return;
                    }
                }

                ranges.push_back(current_range);

                if (current_end_index >= static_cast<int64_t>(range.length() - 1)) {
                    more_ranges_exists = false;
                } else {
                    // Multiple ranges are separated by ', '
                    more_ranges_exists = true;
                    current_start_index = current_end_index + 3;
                }
            } while (more_ranges_exists);
        }

        std::string response = "Download!";
        int status_code = 200;
        std::string headers;

        if (!ranges.empty()) {
            // Create response parts
            std::vector<std::string> responses;
            for (std::pair<int64_t, int64_t> local_range : ranges) {
                if (local_range.first >= 0) {
                    if (local_range.first >= (int64_t) response.length()) {
                        responses.push_back("");
                    } else if (local_range.second == -1 || local_range.second >= (int64_t) response.length()) {
                        responses.push_back(response.substr(local_range.first));
                    } else {
                        responses.push_back(response.substr(local_range.first, local_range.second - local_range.first + 1));
                    }
                }
            }

            if (responses.size() > 1) {
                // Create mime multipart response
                std::string boundary = "3d6b6a416f9b5";
                status_code = 206;
                response.clear();

                for (size_t i{0}; i < responses.size(); ++i) {
                    response += "--" + boundary + "\n";
                    response += "Content-Range: bytes " + std::to_string(ranges.at(i).first) + "-";
                    if (ranges.at(i).second > 0) {
                        response += std::to_string(ranges.at(i).second);
                    } else {
                        response += std::to_string(responses.at(i).length());
                    }
                    response += "/" + std::to_string(responses.at(i).length()) + "\n\n";
                    response += responses.at(i) + "\n";
                }
                response += "--" + boundary + "--";
            } else {
                if (ranges.at(0).second == -1 || ranges.at(0).second >= (int64_t) response.length()) {
                    status_code = ranges.at(0).first > 0 ? 206 : 200;
                } else {
                    status_code = 206;
                }
                response = responses.at(0);

                if (status_code == 206) {
                    headers = "Content-Range: bytes " + std::to_string(ranges.at(0).first) + "-";
                    if (ranges.at(0).second > 0) {
                        headers += std::to_string(ranges.at(0).second);
                    } else {
                        headers += std::to_string(response.length());
                    }
                    headers += "/" + std::to_string(response.length());
                }
            }
        }
        if (!headers.empty()) {
            headers += "\r\n";
        }

        mg_http_reply(conn, status_code, headers.c_str(), response.c_str());
    }
}

void HttpServer::OnRequestCheckAcceptEncoding(mg_connection* conn, mg_http_message* msg) {
    std::string response;
    for (auto& header : msg->headers) {
        if (!header.name.ptr) {
            continue;
        }
        std::string name = std::string(header.name.ptr, header.name.len);
        if (std::string{"Accept-Encoding"} == name) {
            response = std::string(header.value.ptr, header.value.len);
        }
    }
    std::string headers = "Content-Type: text/html\r\n";
    mg_http_reply(conn, 200, headers.c_str(), response.c_str());
}

void HttpServer::OnRequest(mg_connection* conn, mg_http_message* msg) {
    std::string uri = std::string(msg->uri.ptr, msg->uri.len);
    if (uri == "/") {
        OnRequestRoot(conn, msg);
    } else if (uri == "/hello.html") {
        OnRequestHello(conn, msg);
    } else if (uri == "/timeout.html") {
        OnRequestTimeout(conn, msg);
    } else if (uri == "/low_speed_timeout.html") {
        OnRequestLowSpeedTimeout(conn, msg, &mgr);
    } else if (uri == "/low_speed.html") {
        OnRequestLowSpeed(conn, msg, &mgr);
    } else if (uri == "/low_speed_bytes.html") {
        OnRequestLowSpeedBytes(conn, msg, &mgr);
    } else if (uri == "/basic_cookies.html") {
        OnRequestBasicCookies(conn, msg);
    } else if (uri == "/empty_cookies.html") {
        OnRequestEmptyCookies(conn, msg);
    } else if (uri == "/check_cookies.html") {
        OnRequestCheckCookies(conn, msg);
    } else if (uri == "/v1_cookies.html") {
        OnRequestV1Cookies(conn, msg);
    } else if (uri == "/check_v1_cookies.html") {
        OnRequestCheckV1Cookies(conn, msg);
    } else if (uri == "/basic_auth.html") {
        OnRequestBasicAuth(conn, msg);
    } else if (uri == "/bearer_token.html") {
        OnRequestBearerAuth(conn, msg);
    } else if (uri == "/digest_auth.html") {
        OnRequestHeaderReflect(conn, msg);
    } else if (uri == "/basic.json") {
        OnRequestBasicJson(conn, msg);
    } else if (uri == "/header_reflect.html") {
        OnRequestHeaderReflect(conn, msg);
    } else if (uri == "/temporary_redirect.html") {
        OnRequestTempRedirect(conn, msg);
    } else if (uri == "/permanent_redirect.html") {
        OnRequestPermRedirect(conn, msg);
    } else if (uri == "/two_redirects.html") {
        OnRequestTwoRedirects(conn, msg);
    } else if (uri == "/url_post.html") {
        OnRequestUrlPost(conn, msg);
    } else if (uri == "/body_get.html") {
        OnRequestBodyGet(conn, msg);
    } else if (uri == "/json_post.html") {
        OnRequestJsonPost(conn, msg);
    } else if (uri == "/form_post.html") {
        OnRequestFormPost(conn, msg);
    } else if (uri == "/reflect_post.html") {
        OnRequestReflectPost(conn, msg);
    } else if (uri == "/delete.html") {
        OnRequestDelete(conn, msg);
    } else if (uri == "/delete_unallowed.html") {
        OnRequestDeleteNotAllowed(conn, msg);
    } else if (uri == "/put.html") {
        OnRequestPut(conn, msg);
    } else if (uri == "/put_unallowed.html") {
        OnRequestPutNotAllowed(conn, msg);
    } else if (uri == "/patch.html") {
        OnRequestPatch(conn, msg);
    } else if (uri == "/patch_unallowed.html") {
        OnRequestPatchNotAllowed(conn, msg);
    } else if (uri == "/download_gzip.html") {
        OnRequestDownloadGzip(conn, msg);
    } else if (uri == "/local_port.html") {
        OnRequestLocalPort(conn, msg);
    } else if (uri == "/check_accept_encoding.html") {
        OnRequestCheckAcceptEncoding(conn, msg);
    } else {
        OnRequestNotFound(conn, msg);
    }
}

void HttpServer::OnRequestLocalPort(mg_connection* conn, mg_http_message* /*msg*/) {
    // send source port number as response for checking SetLocalPort/SetLocalPortRange
    std::string headers = "Content-Type: text/plain\r\n";
    // Convert from big endian to little endian
    uint16_t remote_port = (conn->rem.port >> 8) | (conn->rem.port << 8);
    std::string response = std::to_string(remote_port);
    mg_http_reply(conn, 200, headers.c_str(), response.c_str());
}

} // namespace cpr
