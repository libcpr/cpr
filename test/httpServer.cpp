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
    time_t expires_time = 3905119080; // Expires=Wed, 30 Sep 2093 03:18:00 GMT
    std::array<char, EXPIRES_STRING_SIZE> expires_string;
    std::strftime(expires_string.data(), expires_string.size(), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&expires_time));

    std::string cookie1{"SID=31d4d96e407aad42; Expires=" + std::string(expires_string.data()) + "; Secure"};
    std::string cookie2{"lang=en-US; Expires=" + std::string(expires_string.data()) + "; Secure"};
    std::string headers =
            "Content-Type: text/html\r\n"
            "Set-Cookie: " +
            cookie1 +
            "\r\n"
            "Set-Cookie: " +
            cookie2;
    std::string response{"Basic Cookies"};

    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestEmptyCookies(mg_connection* conn, http_message* /*msg*/) {
    time_t expires_time = 3905119080; // Expires=Wed, 30 Sep 2093 03:18:00 GMT
    std::array<char, EXPIRES_STRING_SIZE> expires_string;
    std::strftime(expires_string.data(), sizeof(expires_string), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&expires_time));

    std::string cookie1{"SID=; Expires=" + std::string(expires_string.data()) + "; Secure"};
    std::string cookie2{"lang=; Expires=" + std::string(expires_string.data()) + "; Secure"};
    std::string headers =
            "Content-Type: text/html\r\n"
            "Set-Cookie: " +
            cookie1 +
            "\r\n"
            "Set-Cookie: " +
            cookie2;
    std::string response{"Empty Cookies"};

    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

void HttpServer::OnRequestCookiesReflect(mg_connection* conn, http_message* msg) {
    mg_str* request_cookies;
    if ((request_cookies = mg_get_http_header(msg, "Cookie")) == nullptr) {
        mg_http_send_error(conn, 400, "Cookie not found");
        return;
    }
    std::string cookie_str{request_cookies->p, request_cookies->len};
    std::string headers = "Content-Type: text/html";
    mg_send_head(conn, 200, cookie_str.length(), headers.c_str());
    mg_send(conn, cookie_str.c_str(), cookie_str.length());
}

void HttpServer::OnRequestRedirectionWithChangingCookies(mg_connection* conn, http_message* msg) {
    time_t expires_time = 3905119080; // Expires=Wed, 30 Sep 2093 03:18:00 GMT
    std::array<char, EXPIRES_STRING_SIZE> expires_string;
    std::strftime(expires_string.data(), sizeof(expires_string), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&expires_time));

    mg_str* request_cookies;
    std::string cookie_str;
    if ((request_cookies = mg_get_http_header(msg, "Cookie")) != nullptr) {
        cookie_str = std::string{request_cookies->p, request_cookies->len};
    }

    if (cookie_str.find("SID=31d4d96e407aad42") == std::string::npos) {
        std::string cookie1{"SID=31d4d96e407aad42; Expires=" + std::string(expires_string.data()) + "; Secure"};
        std::string cookie2{"lang=en-US; Expires=" + std::string(expires_string.data()) + "; Secure"};
        std::string headers =
                "Content-Type: text/html\r\n"
                "Location: http://127.0.0.1:61936/redirection_with_changing_cookies.html\r\n"
                "Set-Cookie: " +
                cookie1 +
                "\r\n"
                "Set-Cookie: " +
                cookie2;

        mg_send_head(conn, 302, 0, headers.c_str());
        mg_send(conn, NULL, 0);
    } else {
        cookie_str = "Received cookies are: " + cookie_str;
        std::string headers = "Content-Type: text/html";
        mg_send_head(conn, 200, cookie_str.length(), headers.c_str());
        mg_send(conn, cookie_str.c_str(), cookie_str.length());
    }
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
    std::map<std::string, std::string> filenames;

    while ((n2 = mg_parse_multipart(msg->body.p + n1, msg->body.len - n1, var_name, sizeof(var_name), file_name, sizeof(file_name), &chunk, &chunk_len)) > 0) {
        n1 += n2;
        forms[var_name] = std::string(chunk, chunk_len);
        filenames[var_name] = std::string(file_name);
    }

    std::string headers = "Content-Type: application/json";
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

void HttpServer::OnRequestPostReflect(mg_connection* conn, http_message* msg) {
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
        std::vector<std::pair<int64_t, int64_t>> ranges;

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
            return;
        }
        if (!range.empty()) {
            std::string::size_type eq_pos = range.find('=');
            if (eq_pos == std::string::npos) {
                mg_http_send_error(conn, 405, ("Invalid range header: " + range).c_str());
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
                    mg_http_send_error(conn, 405, ("Invalid range format " + range.substr(current_start_index, current_end_index)).c_str());
                    return;
                }
                if (sep_pos == eq_pos + 1) {
                    mg_http_send_error(conn, 405, ("Suffix ranage not supported: " + range.substr(current_start_index, current_end_index)).c_str());
                    return;
                }

                current_range.first = std::strtoll(range.substr(current_start_index, sep_pos - 1).c_str(), nullptr, 10);
                if (current_range.first == LLONG_MAX || current_range.first == LLONG_MIN) {
                    mg_http_send_error(conn, 405, ("Start range is invalid number: " + range.substr(current_start_index, current_end_index)).c_str());
                    return;
                }

                std::string er_str = range.substr(sep_pos + 1, current_end_index);
                if (!er_str.empty()) {
                    current_range.second = std::strtoll(er_str.c_str(), nullptr, 10);
                    if (current_range.second == 0 || current_range.second == LLONG_MAX || current_range.second == LLONG_MIN) {
                        mg_http_send_error(conn, 405, ("End range is invalid number: " + range.substr(current_start_index, current_end_index)).c_str());
                        return;
                    }
                }

                ranges.push_back(current_range);

                if (current_end_index >= (int64_t) (range.length() - 1)) {
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
            for (std::pair<int64_t, int64_t> range : ranges) {
                if (range.first >= 0) {
                    if (range.first >= (int64_t) response.length()) {
                        responses.push_back("");
                    } else if (range.second == -1 || range.second >= (int64_t) response.length()) {
                        responses.push_back(response.substr(range.first));
                    } else {
                        responses.push_back(response.substr(range.first, range.second - range.first + 1));
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

        mg_send_head(conn, status_code, static_cast<int>(response.length()), headers.c_str());
        mg_send(conn, response.c_str(), static_cast<int>(response.length()));
    }
}

void HttpServer::OnRequestCheckAcceptEncoding(mg_connection* conn, http_message* msg) {
    std::string response;
    for (size_t i = 0; i < sizeof(msg->header_names) / sizeof(mg_str); i++) {
        if (!msg->header_names[i].p) {
            continue;
        }
        std::string name = std::string(msg->header_names[i].p, msg->header_names[i].len);
        if (std::string{"Accept-Encoding"} == name) {
            response = std::string(msg->header_values[i].p, msg->header_values[i].len);
        }
    }
    std::string headers = "Content-Type: text/html";
    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
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
    } else if (uri == "/cookies_reflect.html") {
        OnRequestCookiesReflect(conn, msg);
    } else if (uri == "/redirection_with_changing_cookies.html") {
        OnRequestRedirectionWithChangingCookies(conn, msg);
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
    } else if (uri == "/post_reflect.html") {
        OnRequestPostReflect(conn, msg);
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

void HttpServer::OnRequestLocalPort(mg_connection* conn, http_message* /*msg*/) {
    // send source port number as response for checking SetLocalPort/SetLocalPortRange
    std::string headers = "Content-Type: text/plain";
    char portbuf[8];
    mg_conn_addr_to_str(conn, portbuf, sizeof(portbuf), MG_SOCK_STRINGIFY_PORT | MG_SOCK_STRINGIFY_REMOTE);
    std::string response = portbuf;
    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

} // namespace cpr
