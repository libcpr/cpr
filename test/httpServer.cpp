#include "httpServer.hpp"
#include <chrono>
#include <iostream>
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

mg_connection* HttpServer::initServer(mg_mgr* mgr, MG_CB(mg_event_handler_t event_handler, void* user_data)) {
    // Based on: https://cesanta.com/docs/http/server-example.html
    mg_mgr_init(mgr, this);
    std::string port = std::to_string(GetPort());
    mg_connection* c = mg_bind(mgr, port.c_str(), event_handler);
    if (!c) {
        throw std::system_error(errno, std::system_category(), "Failed to bind to port " + port);
    }
    mg_set_protocol_http_websocket(c);
    return c;
}

void HttpServer::OnRequestHello(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p, msg->method.len} == std::string{"OPTIONS"}) {
        OnRequestOptions(conn, msg);
    } else {
        std::string response{"Hello world!"};
        std::string headers = "Content-Type: text/html";
        mg_send_head(conn, 200, response.length(), headers.c_str());
        mg_send(conn, response.c_str(), response.length());
    }
}

void HttpServer::OnRequestRoot(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p, msg->method.len} == std::string{"OPTIONS"}) {
        OnRequestOptions(conn, msg);
    } else {
        mg_http_send_error(conn, 405, "Method Not Allowed");
    }
}

void HttpServer::OnRequestNotFound(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p, msg->method.len} == std::string{"OPTIONS"}) {
        OnRequestOptions(conn, msg);
    } else {
        mg_http_send_error(conn, 404, "Not Found");
    }
}

void HttpServer::OnRequestOptions(mg_connection* conn, http_message* /*msg*/) {
    std::string headers =
            "Content-Type: text/plain\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Credentials: true\r\n"
            "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, PATCH, OPTIONS\r\n"
            "Access-Control-Max-Age: 3600";

    mg_send_head(conn, 200, 0, headers.c_str());
    std::string response;
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestTimeout(mg_connection* conn, http_message* msg) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestLowSpeedTimeout(mg_connection* conn, http_message* /*msg*/) {
    std::string response{"Hello world!"};
    std::string headers = "Content-Type: text/html";
    mg_send_head(conn, 200, response.length() * 20, headers.c_str());
    for (size_t i = 0; i < 20; i++) {
        mg_send(conn, response.c_str(), response.length());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void HttpServer::OnRequestLowSpeed(mg_connection* conn, http_message* /*msg*/) {
    std::string response{"Hello world!"};
    std::string headers = "Content-Type: text/html";
    mg_send_head(conn, 200, response.length(), headers.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(2));
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestLowSpeedBytes(mg_connection* conn, http_message* /*msg*/) {
    std::string response{"a"};
    std::string headers = "Content-Type: text/html";
    mg_send_head(conn, 200, response.length(), headers.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(2));
    for (size_t i = 0; i < 20; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        mg_send(conn, response.c_str(), response.length());
    }
}

void HttpServer::OnRequestBasicCookies(mg_connection* conn, http_message* /*msg*/) {
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

void HttpServer::OnRequestEmptyCookies(mg_connection* conn, http_message* /*msg*/) {
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

    if (cookie_str.find("cookie=chocolate;") == cookie_str.npos || cookie_str.find("icecream=vanilla;") == cookie_str.npos) {
        mg_http_send_error(conn, 400, "Cookies not found");
    }

    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestV1Cookies(mg_connection* conn, http_message* /*msg*/) {
    time_t t = time(nullptr) + 5; // Valid for 1 hour
    char expire[100], expire_epoch[100];
    snprintf(expire_epoch, sizeof(expire_epoch), "%lu", static_cast<unsigned long>(t));
    strftime(expire, sizeof(expire), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
    std::string v1cookie{"cookie=\"value with spaces (v1 cookie)\"; expires=\"" + std::string{expire} + "\"; http-only;"};

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

    if (cookie_str.find("cookie=\"value with spaces (v1 cookie)\";") == std::string::npos) {
        mg_http_send_error(conn, 400, "Cookie with space not found");
        return;
    }

    OnRequestHello(conn, msg);
}

void HttpServer::OnRequestBasicAuth(mg_connection* conn, http_message* msg) {
    mg_str* requested_auth;
    std::string auth{"Basic"};
    if ((requested_auth = mg_get_http_header(msg, "Authorization")) == nullptr || mg_ncasecmp(requested_auth->p, auth.c_str(), auth.length()) != 0) {
        mg_http_send_error(conn, 401, "Unauthorized");
        return;
    }
    std::string auth_string{requested_auth->p, requested_auth->len};
    size_t basic_token = auth_string.find(' ') + 1;
    auth_string = auth_string.substr(basic_token, auth_string.length() - basic_token);
    auth_string = Base64Decode(auth_string);
    size_t colon = auth_string.find(':');
    std::string username = auth_string.substr(0, colon);
    std::string password = auth_string.substr(colon + 1, auth_string.length() - colon - 1);
    if (username == "user" && password == "password") {
        OnRequestHeaderReflect(conn, msg);
    } else {
        mg_http_send_error(conn, 401, "Unauthorized");
    }
}

void HttpServer::OnRequestBearerAuth(mg_connection* conn, http_message* msg) {
    mg_str* requested_auth;
    std::string auth{"Bearer"};
    if ((requested_auth = mg_get_http_header(msg, "Authorization")) == nullptr || mg_ncasecmp(requested_auth->p, auth.c_str(), auth.length()) != 0) {
        mg_http_send_error(conn, 401, "Unauthorized");
        return;
    }
    std::string auth_string{requested_auth->p, requested_auth->len};
    size_t basic_token = auth_string.find(' ') + 1;
    auth_string = auth_string.substr(basic_token, auth_string.length() - basic_token);
    if (auth_string == "the_token") {
        OnRequestHeaderReflect(conn, msg);
    } else {
        mg_http_send_error(conn, 401, "Unauthorized");
    }
}

void HttpServer::OnRequestBasicJson(mg_connection* conn, http_message* /*msg*/) {
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
    std::string headers;
    bool hasContentTypeHeader = false;
    for (size_t i = 0; i < sizeof(msg->header_names) / sizeof(mg_str); i++) {
        if (!msg->header_names[i].p) {
            continue;
        }

        std::string name = std::string(msg->header_names[i].p, msg->header_names[i].len);
        if (std::string{"Content-Type"} == name) {
            hasContentTypeHeader = true;
        }

        if (std::string{"Host"} != name && std::string{"Accept"} != name) {
            if (!headers.empty()) {
                headers.append("\r\n");
            }
            if (msg->header_values[i].p) {
                headers.append(name + ": " + std::string(msg->header_values[i].p, msg->header_values[i].len));
            }
        }
    }

    if (!hasContentTypeHeader) {
        if (!headers.empty()) {
            headers.append("\r\n");
        }
        headers.append("Content-Type: text/html");
    }
    std::cout << "HEADERS: " << headers << '\n';
    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestTempRedirect(mg_connection* conn, http_message* msg) {
    // Get the requested target location:
    std::string location;
    for (size_t i = 0; i < sizeof(msg->header_names) / sizeof(mg_str); i++) {
        if (!msg->header_names[i].p) {
            continue;
        }

        std::string name = std::string(msg->header_names[i].p, msg->header_names[i].len);
        if (std::string{"RedirectLocation"} == name) {
            location = std::string(msg->header_values[i].p, msg->header_values[i].len);
            break;
        }
    }

    // Check if the request contains a valid location, else default to 'hello.html':
    if (location.empty()) {
        location = "hello.html";
    }
    std::string headers = "Location: " + location;
    std::string response = "Moved Temporarily";
    mg_send_head(conn, 302, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestPermRedirect(mg_connection* conn, http_message* msg) {
    // Get the requested target location:
    std::string location;
    for (size_t i = 0; i < sizeof(msg->header_names) / sizeof(mg_str); i++) {
        if (!msg->header_names[i].p) {
            continue;
        }

        std::string name = std::string(msg->header_names[i].p, msg->header_names[i].len);
        if (std::string{"RedirectLocation"} == name) {
            location = std::string(msg->header_values[i].p, msg->header_values[i].len);
            break;
        }
    }

    // Check if the request contains a valid location, else default to 'hello.html':
    if (location.empty()) {
        location = "hello.html";
    }
    std::string headers = "Location: " + location;
    std::string response = "Moved Permanently";

    mg_send_head(conn, 301, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestTwoRedirects(mg_connection* conn, http_message* /*msg*/) {
    std::string response = "Moved Permanently";
    std::string headers = "Location: permanent_redirect.html";
    mg_send_head(conn, 301, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestUrlPost(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p, msg->method.len} != std::string{"POST"}) {
        mg_http_send_error(conn, 405, "Method Not Allowed");
        return;
    }

    std::string headers = "Content-Type: application/json";

    char x[100];
    char y[100];
    mg_get_http_var(&(msg->body), "x", x, sizeof(x));
    mg_get_http_var(&(msg->body), "y", y, sizeof(y));
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
    mg_send_head(conn, 201, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestBodyGet(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p, msg->method.len} != std::string{"GET"}) {
        mg_http_send_error(conn, 405, "Method Not Allowed");
        return;
    }
    std::array<char, 100> message{};
    mg_get_http_var(&(msg->body), "message", message.data(), message.size());
    if (msg->body.len <= 0) {
        mg_http_send_error(conn, 405, "No Content");
        return;
    }
    std::string response = message.data();
    std::string headers = "Content-Type: text/html";
    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestJsonPost(mg_connection* conn, http_message* msg) {
    mg_str* content_type{nullptr};
    if ((content_type = mg_get_http_header(msg, "Content-Type")) == nullptr || std::string{content_type->p, content_type->len} != "application/json") {
        mg_http_send_error(conn, 415, "Unsupported Media Type");
        return;
    }

    std::string headers = "Content-Type: application/json";
    mg_send_head(conn, 201, msg->body.len, headers.c_str());
    mg_send(conn, msg->body.p, msg->body.len);
}

void HttpServer::OnRequestFormPost(mg_connection* conn, http_message* msg) {
    char var_name[100];
    char file_name[100];
    const char* chunk;
    size_t chunk_len = 0;
    size_t n1 = 0;
    size_t n2 = 0;
    std::map<std::string, std::string> forms;

    while ((n2 = mg_parse_multipart(msg->body.p + n1, msg->body.len - n1, var_name, sizeof(var_name), file_name, sizeof(file_name), &chunk, &chunk_len)) > 0) {
        n1 += n2;
        forms[var_name] = std::string(chunk, chunk_len);
    }

    std::string x = forms["x"];

    std::string headers = "Content-Type: application/json";
    std::string response;
    if (forms.find("y") == forms.end()) {
        response = std::string{
                "{\n"
                "  \"x\": " +
                forms["x"] +
                "\n"
                "}"};
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
    mg_send_head(conn, 201, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestDelete(mg_connection* conn, http_message* msg) {
    bool has_json_header = false;
    for (size_t i = 0; i < sizeof(msg->header_names) / sizeof(mg_str); i++) {
        if (!msg->header_names[i].p) {
            continue;
        }

        std::string name = std::string(msg->header_names[i].p, msg->header_names[i].len);
        std::string value = std::string(msg->header_values[i].p, msg->header_values[i].len);
        if (std::string{"Content-Type"} == name && std::string{"application/json"} == value) {
            has_json_header = true;
            break;
        }
    }
    if (std::string{msg->method.p, msg->method.len} == std::string{"DELETE"}) {
        std::string headers;
        std::string response = "Patch success";
        if (!has_json_header) {
            headers = "Content-Type: text/html";
            response = "Delete success";
        } else {
            headers = "Content-Type: application/json";
            response = std::string{msg->body.p, msg->body.len};
        }
        mg_send_head(conn, 200, response.length(), headers.c_str());
        mg_send(conn, response.c_str(), response.length());
    } else {
        mg_http_send_error(conn, 405, "Method Not Allowed");
    }
}

void HttpServer::OnRequestDeleteNotAllowed(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p, msg->method.len} == std::string{"DELETE"}) {
        mg_http_send_error(conn, 405, "Method Not Allowed");
    } else {
        std::string headers = "Content-Type: text/html";
        std::string response = "Delete success";
        mg_send_head(conn, 200, response.length(), headers.c_str());
        mg_send(conn, response.c_str(), response.length());
    }
}

void HttpServer::OnRequestPut(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p, msg->method.len} == std::string{"PUT"}) {
        char x[100];
        char y[100];
        mg_get_http_var(&(msg->body), "x", x, sizeof(x));
        mg_get_http_var(&(msg->body), "y", y, sizeof(y));
        std::string x_string = std::string{x};
        std::string y_string = std::string{y};
        std::string headers = "Content-Type: application/json";
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
        mg_send_head(conn, 200, response.length(), headers.c_str());
        mg_send(conn, response.c_str(), response.length());
    } else {
        mg_http_send_error(conn, 405, "Method Not Allowed");
    }
}

void HttpServer::OnRequestReflectPost(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p, msg->method.len} != std::string{"POST"}) {
        mg_http_send_error(conn, 405, "Method Not Allowed");
    }

    std::string response = std::string{msg->body.p, msg->body.len};
    std::string headers;
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
                headers.append(name + ": " + std::string(msg->header_values[i].p, msg->header_values[i].len));
            }
        }
    }
    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestPutNotAllowed(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p, msg->method.len} == std::string{"PUT"}) {
        mg_http_send_error(conn, 405, "Method Not Allowed");
    } else {
        std::string headers = "Content-Type: text/html";
        std::string response = "Delete success";
        mg_send_head(conn, 200, response.length(), headers.c_str());
        mg_send(conn, response.c_str(), response.length());
    }
}

void HttpServer::OnRequestPatch(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p, msg->method.len} == std::string{"PATCH"}) {
        char x[100];
        char y[100];
        mg_get_http_var(&(msg->body), "x", x, sizeof(x));
        mg_get_http_var(&(msg->body), "y", y, sizeof(y));
        std::string x_string = std::string{x};
        std::string y_string = std::string{y};
        std::string headers = "Content-Type: application/json";
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
        mg_send_head(conn, 200, response.length(), headers.c_str());
        mg_send(conn, response.c_str(), response.length());
    } else {
        mg_http_send_error(conn, 405, "Method Not Allowed");
    }
}

void HttpServer::OnRequestPatchNotAllowed(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p, msg->method.len} == std::string{"PATCH"}) {
        mg_http_send_error(conn, 405, "Method Not Allowed");
    } else {
        std::string headers = "Content-Type: text/html";
        std::string response = "Delete success";
        mg_send_head(conn, 200, response.length(), headers.c_str());
        mg_send(conn, response.c_str(), response.length());
    }
}

void HttpServer::OnRequestDownloadGzip(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p, msg->method.len} == std::string{"DOWNLOAD"}) {
        mg_http_send_error(conn, 405, "Method Not Allowed");
    } else {
        std::string encoding;
        std::string range;
        int64_t srange = 0;
        int64_t erange = -1;
        for (size_t i = 0; i < sizeof(msg->header_names) / sizeof(mg_str); i++) {
            if (!msg->header_names[i].p) {
                continue;
            }

            std::string name = std::string(msg->header_names[i].p, msg->header_names[i].len);
            if (std::string{"Accept-Encoding"} == name) {
                encoding = std::string(msg->header_values[i].p, msg->header_values[i].len);
            } else if (std::string{"Range"} == name) {
                range = std::string(msg->header_values[i].p, msg->header_values[i].len);
            }
        }
        if (encoding.find("gzip") == std::string::npos) {
            mg_http_send_error(conn, 405, ("Invalid encoding: " + encoding).c_str());
        } else {
            if ( !range.empty() ) {
                std::string::size_type eq_pos = range.find('=');
                if (eq_pos == std::string::npos) {
                    mg_http_send_error(conn, 405, ("Invalid range header: " + range).c_str());
                    return;
                }
                std::string::size_type sep_pos = range.find('-', eq_pos + 1);
                if (sep_pos == std::string::npos) {
                    mg_http_send_error(conn, 405, ("Invalid range format " + range).c_str());
                    return;
                }
                if (sep_pos != std::string::npos) {
                    if (sep_pos == eq_pos + 1) {
                        mg_http_send_error(conn, 405, ("Suffix ranage not supported: " + range).c_str());
                        return;
                    }
                    srange = std::strtoll(range.substr(eq_pos + 1, sep_pos - eq_pos - 1).c_str(), nullptr, 10);
                    if (srange == LLONG_MAX || srange == LLONG_MIN) {
                        mg_http_send_error(conn, 405, ("Start range is invalid number: " + range).c_str());
                        return;
                    }
                    std::string::size_type coma_pos = range.find(',', sep_pos + 1);
                    std::string er_str = range.substr(sep_pos + 1, coma_pos);
                    if (!er_str.empty()) {
                        erange = std::strtoll(er_str.c_str(), nullptr, 10);
                        if (erange == 0 || erange == LLONG_MAX || erange == LLONG_MIN) {
                            mg_http_send_error(conn, 405, ("End range is invalid number: " + range).c_str());
                            return;
                        }
                    }
                }
            }
            std::string response = "Download!";
            int status_code = 200;
            if (srange >= 0) {
                int64_t resp_len = response.length();
                if (srange >= resp_len) {
                    response.clear();
                    status_code = 206;
                } else if (erange == -1 || erange >= resp_len ) {
                    response = response.substr(srange);
                    status_code = srange > 0 ? 206 : 200;
                } else {
                    response = response.substr(srange, erange - srange + 1);
                    status_code = 206;
                }
            }
            std::string headers;
            if (status_code == 206) {
                headers = "Content-Range: bytes " + std::to_string(srange) + "-";
                if (erange > 0) {
                    headers += std::to_string(erange);
                } else {
                    headers += std::to_string(response.length());
                }
                headers += "/" + std::to_string(response.length());
            }
            mg_send_head(conn, status_code, static_cast<int>(response.length()), headers.c_str());
            mg_send(conn, response.c_str(), static_cast<int>(response.length()));
        }
    }
}

void HttpServer::OnRequest(mg_connection* conn, http_message* msg) {
    std::string uri = std::string(msg->uri.p, msg->uri.len);
    if (uri == "/") {
        OnRequestRoot(conn, msg);
    } else if (uri == "/hello.html") {
        OnRequestHello(conn, msg);
    } else if (uri == "/timeout.html") {
        OnRequestTimeout(conn, msg);
    } else if (uri == "/low_speed_timeout.html") {
        OnRequestLowSpeedTimeout(conn, msg);
    } else if (uri == "/low_speed.html") {
        OnRequestLowSpeed(conn, msg);
    } else if (uri == "/low_speed_bytes.html") {
        OnRequestLowSpeedBytes(conn, msg);
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
    } else {
        OnRequestNotFound(conn, msg);
    }
}

} // namespace cpr
