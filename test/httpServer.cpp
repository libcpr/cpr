#include "httpServer.hpp"

namespace cpr {
void HttpServer::SetUp() {
    Start();
}

void HttpServer::TearDown() {
    Stop();
}

void HttpServer::Start() {
    should_run = true;
    serverThread = std::make_shared<std::thread>(&HttpServer::Run, this);
    serverThread->detach();
    std::unique_lock<std::mutex> server_lock(server_mutex);
    server_start_cv.wait(server_lock);
}

void HttpServer::Stop() {
    should_run = false;
    std::unique_lock<std::mutex> server_lock(server_mutex);
    server_stop_cv.wait(server_lock);
}

Url HttpServer::GetBaseUrl() {
    return Url{"http://127.0.0.1:"}.append(std::to_string(GetPort()));
}

Url HttpServer::GetBaseUrlSSL() {
    return Url{"http://127.0.0.1:"}.append(std::to_string(GetPort()));
}

uint16_t HttpServer::GetPort() {
    return 8080;
}

void HttpServer::OnRequestHello(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p} == std::string{"OPTIONS"}) {
        OnRequestOptions(conn, msg);
    } else {
        std::string response{"Hello world!"};
        std::string headers = "Content-Type: text/html";
        mg_send_head(conn, 200, response.length(), headers.c_str());
        mg_send(conn, response.c_str(), response.length());
    }
}

void HttpServer::OnRequestRoot(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p} == std::string{"OPTIONS"}) {
        OnRequestOptions(conn, msg);
    } else {
        mg_http_send_error(conn, 405, "Method Not Allowed");
    }
}

void HttpServer::OnRequestNotFound(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p} == std::string{"OPTIONS"}) {
        OnRequestOptions(conn, msg);
    } else {
        mg_http_send_error(conn, 404, "Not Found");
    }
}

void HttpServer::OnRequestOptions(mg_connection* conn, http_message* msg) {
    std::string headers =
            "Content-Type: text/plain\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Credentials: true\r\n"
            "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, PATCH, OPTIONS\r\n"
            "Access-Control-Max-Age: 3600";

    mg_send_head(conn, 200, 0, headers.c_str());
    std::string response{""};
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestTimeout(mg_connection* conn, http_message* msg) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestLowSpeed(mg_connection* conn, http_message* msg) {
    std::string response{"Hello world!"};
    std::string headers = "Content-Type: text/html";
    mg_send_head(conn, 200, response.length(), headers.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(2));
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestLowSpeedBytes(mg_connection* conn, http_message* msg) {
    std::string response{"a"};
    std::string headers = "Content-Type: text/html";
    mg_send_head(conn, 200, response.length(), headers.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(2));
    for (auto i = 0; i < 20; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        mg_send(conn, response.c_str(), response.length());
    }
}

void HttpServer::OnRequestBasicCookies(mg_connection* conn, http_message* msg) {
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
            cookie2;
    std::string response{"Hello world!"};
    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestCheckCookies(mg_connection* conn, http_message* msg) {
    mg_str* request_cookies;
    if ((request_cookies = mg_get_http_header(msg, "Cookie")) == nullptr) {
        mg_http_send_error(conn, 400, "Cookie not found");
        return;
    }
    std::string cookie_str{request_cookies->p, request_cookies->len};

    if (cookie_str.find("cookie=chocolate;") == cookie_str.npos ||
        cookie_str.find("icecream=vanilla;") == cookie_str.npos) {
        mg_http_send_error(conn, 400, "Cookies not found");
    }

    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestV1Cookies(mg_connection* conn, http_message* msg) {
    time_t t = time(nullptr) + 5; // Valid for 1 hour
    char expire[100], expire_epoch[100];
    snprintf(expire_epoch, sizeof(expire_epoch), "%lu", static_cast<unsigned long>(t));
    strftime(expire, sizeof(expire), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
    std::string v1cookie{"cookie=\"value with spaces (v1 cookie)\"; expires=\"" +
                         std::string{expire} + "\"; http-only;"};

    std::string headers =
            "Content-Type: text/html\r\n"
            "Set-Cookie: " +
            v1cookie;
    std::string response{"Hello world!"};
    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestCheckV1Cookies(mg_connection* conn, http_message* msg) {
    mg_str* request_cookies;
    if ((request_cookies = mg_get_http_header(msg, "Cookie")) == nullptr) {
        mg_http_send_error(conn, 400, "Cookie not found");
        return;
    }
    std::string cookie_str{request_cookies->p, request_cookies->len};

    if (cookie_str.find("cookie=\"value with spaces (v1 cookie)\";") == cookie_str.npos) {
        mg_http_send_error(conn, 400, "Cookie with space not found");
        return;
    }

    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestBasicAuth(mg_connection* conn, http_message* msg) {
    mg_str* requested_auth;
    auto auth = std::string{"Basic"};
    if ((requested_auth = mg_get_http_header(msg, "Authorization")) == nullptr ||
        mg_ncasecmp(requested_auth->p, auth.data(), auth.length()) != 0) {
        mg_http_send_error(conn, 401, "Unauthorized");
        return;
    }
    auto auth_string = std::string{requested_auth->p, requested_auth->len};
    auto basic_token = auth_string.find(' ') + 1;
    auth_string = auth_string.substr(basic_token, auth_string.length() - basic_token);
    auth_string = Base64Decode(auth_string);
    auto colon = auth_string.find(':');
    auto username = auth_string.substr(0, colon);
    auto password = auth_string.substr(colon + 1, auth_string.length() - colon - 1);
    if (username == "user" && password == "password") {
        OnRequestHeaderReflect(conn, msg);
    } else {
        mg_http_send_error(conn, 401, "Unauthorized");
    }
}

void HttpServer::OnRequestDigestAuth(mg_connection* conn, http_message* msg) {
    // Temporary:
    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestBasicJson(mg_connection* conn, http_message* msg) {
    std::string response =
            "[\n"
            "  {\n"
            "    \"first_key\": \"first_value\",\n"
            "    \"second_key\": \"second_value\"\n"
            "  }\n"
            "]";
    std::string headers = "Content-Type: application/json";
    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestHeaderReflect(mg_connection* conn, http_message* msg) {
    std::string response = "Header reflect " + std::string{msg->method.p, msg->method.len};
    std::string headers = "Content-Type: text/html";
    for (size_t i = 0; i < sizeof(msg->header_names) / sizeof(mg_str); i++) {
        if (!msg->header_names[i].p) {
            continue;
        }

        std::string name = std::string(msg->header_names[i].p, msg->header_names[i].len);
        if (std::string{"Host"} != name && std::string{"Accept"} != name) {
            if (!headers.empty()) {
                headers.append("\r\n");
            }
            if (msg->header_values[i].p) {
                headers.append(name + ": " +
                               std::string(msg->header_values[i].p, msg->header_values[i].len));
            }
        }
    }
    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.data(), response.length());
}

void HttpServer::OnRequestTempRedirect(mg_connection* conn, http_message* msg) {
    std::string response = "Moved Temporarily";
    std::string headers = "Location: hello.html";
    mg_send_head(conn, 302, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestPermRedirect(mg_connection* conn, http_message* msg) {
    std::string response = "Moved Permanently";
    std::string headers = "Location: hello.html";
    mg_send_head(conn, 301, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestTwoRedirects(mg_connection* conn, http_message* msg) {
    std::string response = "Moved Permanently";
    std::string headers = "Location: permanent_redirect.html";
    mg_send_head(conn, 301, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestUrlPost(mg_connection* conn, http_message* msg) {
    std::string headers = "Content-Type: application/json";

    char x[100];
    char y[100];
    mg_get_http_var(&(msg->body), "x", x, sizeof(x));
    mg_get_http_var(&(msg->body), "y", y, sizeof(y));
    auto x_string = std::string{x};
    auto y_string = std::string{y};
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
    mg_send_head(conn, 201, response.length(), headers.c_str());
    mg_send(conn, response.data(), response.length());
}

void HttpServer::OnRequestBodyGet(mg_connection* conn, http_message* msg) {
    char message[100];
    mg_get_http_var(&(msg->body), "message", message, sizeof(message));
    std::string response = message;
    std::string headers = "Content-Type: text/html";
    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestJsonPost(mg_connection* conn, http_message* msg) {
    mg_str* content_type;
    if ((content_type = mg_get_http_header(msg, "Content-Type")) == nullptr ||
        std::string{content_type->p, content_type->len} != "application/json") {
        mg_http_send_error(conn, 415, "Unsupported Media Type");
        return;
    }

    std::string headers = "Content-Type: application/json";
    mg_send_head(conn, 201, msg->body.len, headers.c_str());
    mg_send(conn, msg->body.p, msg->body.len);
}

void HttpServer::OnRequestFormPost(mg_connection* conn, http_message* msg) {
    // Temporary:
    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestDelete(mg_connection* conn, http_message* msg) {
    // Temporary:
    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestDeleteNotAllowed(mg_connection* conn, http_message* msg) {
    // Temporary:
    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestPut(mg_connection* conn, http_message* msg) {
    // Temporary:
    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestPutNotAllowed(mg_connection* conn, http_message* msg) {
    // Temporary:
    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestPatchNotAllowed(mg_connection* conn, http_message* msg) {
    // Temporary:
    OnRequestHello(conn, msg);
}


void HttpServer::OnRequest(mg_connection* conn, http_message* msg) {
    std::string uri = std::string(msg->uri.p, msg->uri.len);
    if (uri == "/") {
    } else if (uri == "/hello.html") {
        OnRequestHello(conn, msg);
    } else if (uri == "/timeout.html") {
        OnRequestTimeout(conn, msg);
    } else if (uri == "/low_speed.html") {
        OnRequestLowSpeed(conn, msg);
    } else if (uri == "/low_speed_bytes.html") {
        OnRequestLowSpeedBytes(conn, msg);
    } else if (uri == "/basic_cookies.html") {
        OnRequestBasicCookies(conn, msg);
    } else if (uri == "/check_cookies.html") {
        OnRequestCheckCookies(conn, msg);
    } else if (uri == "/v1_cookies.html") {
        OnRequestV1Cookies(conn, msg);
    } else if (uri == "/check_v1_cookies.html") {
        OnRequestCheckV1Cookies(conn, msg);
    } else if (uri == "/basic_auth.html") {
        OnRequestBasicAuth(conn, msg);
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
    } else if (uri == "/delete.html") {
        Delete(conn, msg);
    } else if (uri == "/delete_unallowed.html") {
        OnRequestDeleteNotAllowed(conn, msg);
    } else if (uri == "/put.html") {
        OnRequestPut(conn, msg);
    } else if (uri == "/put_unallowed.html") {
        OnRequestPutNotAllowed(conn, msg);
    } else if (uri == "/patch.html") {
        OnRequestPatchNotAllowed(conn, msg);
    } else if (uri == "/patch_unallowed.html") {
        OnRequestPatchNotAllowed(conn, msg);
    } else {
        OnRequestNotFound(conn, msg);
    }
}

static void EventHandler(mg_connection* conn, int event, void* event_data) {
    switch (event) {
        case MG_EV_RECV:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_SEND:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_POLL:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_CLOSE:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_ACCEPT:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_CONNECT:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_TIMER:
            /** Do nothing. Just for housekeeping. **/
            break;

        case MG_EV_HTTP_CHUNK:
            /** Do nothing. Just for housekeeping. **/
            break;

        case MG_EV_HTTP_REQUEST: {
            HttpServer* server = static_cast<HttpServer*>(conn->mgr_data);
            server->OnRequest(conn, static_cast<http_message*>(event_data));
        } break;

        default:
            break;
    }
}

void HttpServer::Run() {
    // Setup a new mongoose http server.
    // Based on: https://cesanta.com/docs/http/server-example.html
    mg_mgr mgr;
    mg_connection* c;
    mg_mgr_init(&mgr, this);
    std::string port = std::to_string(GetPort());
    c = mg_bind(&mgr, port.c_str(), EventHandler);
    mg_set_protocol_http_websocket(c);

    // Notify the main thread that the server is up and runing:
    server_start_cv.notify_all();

    // Main server loop:
    while (should_run) {
        mg_mgr_poll(&mgr, 1000);
    }

    // Shutdown and cleanup:
    mg_mgr_free(&mgr);

    // Notify the main thread that we have shut down everything:
    server_stop_cv.notify_all();
}

static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
/**
 * Decodes the given BASE64 string to a normal string.
 * Source: https://gist.github.com/williamdes/308b95ac9ef1ee89ae0143529c361d37
 **/
std::string HttpServer::Base64Decode(const std::string& in) {
    std::string out;

    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++)
        T[base64_chars[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1)
            break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

static int LowerCase(const char* s) {
    return tolower(*reinterpret_cast<const unsigned char*>(s));
}

static int StrnCaseCmp(const char* s1, const char* s2, size_t len) {
    int diff = 0;

    if (len > 0) {
        do {
            diff = LowerCase(s1++) - LowerCase(s2++);
        } while (diff == 0 && s1[-1] != '\0' && --len > 0);
    }

    return diff;
}

} // namespace cpr
