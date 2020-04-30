#include "server.h"

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <sstream>
#include <thread>
#include <memory>

// https://cesanta.com/docs/http/server-example.html

#include "mongoose/mongoose.h"

#include <time.h>

#define SERVER_PORT std::string{"8080"}

std::mutex shutdown_mutex;
std::mutex server_mutex;
std::condition_variable server_cv;

static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

static inline bool is_base64(unsigned char c);
std::string base64_decode(const std::string& encoded_string);
static int lowercase(const char *s);
static int mg_strncasecmp(const char *s1, const char *s2, size_t len);

static void options(mg_connection* conn, http_message* msg) {
    if (std::string{msg->method.p} == std::string{"OPTIONS"}) {
        std::string headers = "Content-Type: text/plain\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Credentials: true"
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, PATCH, OPTIONS"
        "Access-Control-Max-Age: 3600";
        mg_send_head(conn, 200, 0, headers.c_str());
        std::string response;
        mg_printf(conn, "%.*s", response.length(), response.c_str());
    } else {
        std::string response{"Method unallowed"};
        std::string headers = "Content-Type: text/plain\r\n";
        mg_send_head(conn, 405, response.length(), headers.c_str());
        mg_printf(conn, "%.*s", response.length(), response.c_str());
    }
}

/*
static int hello(mg_connection* conn) {
    if (std::string{conn->request_method} == std::string{"OPTIONS"}) {
        auto response = std::string{""};
        mg_send_status(conn, 200);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_header(conn, "Access-Control-Allow-Origin", "*");
        mg_send_header(conn, "Access-Control-Allow-Credentials", "true");
        mg_send_header(conn, "Access-Control-Allow-Methods", "GET, OPTIONS");
        mg_send_header(conn, "Access-Control-Max-Age", "3600");
        mg_send_data(conn, response.data(), response.length());
    } else {
        auto response = std::string{"Hello world!"};
        mg_send_status(conn, 200);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int timeout(mg_connection* conn) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto response = std::string{"Hello world!"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int lowSpeed(mg_connection* conn) {
    auto response = std::string{"Hello world!"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int lowSpeedBytes(mg_connection* conn) {
    auto response = std::string{"a"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    for (auto i = 0; i < 20; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int basicCookies(mg_connection* conn) {
    auto response = std::string{"Hello world!"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    time_t t = time(nullptr) + 5;  // Valid for 1 hour
    char expire[100], expire_epoch[100];
    snprintf(expire_epoch, sizeof(expire_epoch), "%lu", static_cast<unsigned long>(t));
    strftime(expire, sizeof(expire), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
    std::string cookie{"cookie=chocolate; expires=\"" + std::string{expire} + "\"; http-only;"};
    std::string cookie2{"icecream=vanilla; expires=\"" + std::string{expire} + "\"; http-only;"};
    mg_send_header(conn, "Set-Cookie", cookie.data());
    mg_send_header(conn, "Set-Cookie", cookie2.data());
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int v1Cookies(mg_connection* conn) {
    auto response = std::string{"Hello world!"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    time_t t = time(nullptr) + 5; // Valid for 1 hour
    char expire[100], expire_epoch[100];
    snprintf(expire_epoch, sizeof(expire_epoch), "%lu", static_cast<unsigned long>(t));
    strftime(expire, sizeof(expire), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
    std::string v1cookie{"cookie=\"value with spaces (v1 cookie)\"; expires=\"" +
                         std::string{expire} + "\"; http-only;"};
    mg_send_header(conn, "Set-Cookie", v1cookie.data());
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int checkBasicCookies(mg_connection* conn) {
    const char* request_cookies;
    if ((request_cookies = mg_get_header(conn, "Cookie")) == nullptr)
        return MG_FALSE;
    std::string cookie_str{request_cookies};

    if (cookie_str.find("cookie=chocolate;") == cookie_str.npos ||
        cookie_str.find("icecream=vanilla;") == cookie_str.npos) {
        return MG_FALSE;
    }

    auto response = std::string{"Hello world!"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int checkV1Cookies(mg_connection* conn) {
    const char* request_cookies;
    if ((request_cookies = mg_get_header(conn, "Cookie")) == nullptr)
        return MG_FALSE;
    std::string cookie_str{request_cookies};

    if (cookie_str.find("cookie=\"value with spaces (v1 cookie)\";") == cookie_str.npos) {
        return MG_FALSE;
    }

    auto response = std::string{"Hello world!"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static void basicAuth(mg_connection* conn) {
    auto response = std::string{"Hello world!"};
    const char* requested_auth;
    auto auth = std::string{"Basic"};
    if ((requested_auth = mg_get_http_header(conn, "Authorization")) == nullptr ||
        mg_strncasecmp(requested_auth, auth.data(), auth.length()) != 0) {
        return;
    }
    auto auth_string = std::string{requested_auth};
    auto basic_token = auth_string.find(' ') + 1;
    auth_string = auth_string.substr(basic_token, auth_string.length() - basic_token);
    auth_string = base64_decode(auth_string);
    auto colon = auth_string.find(':');
    auto username = auth_string.substr(0, colon);
    auto password = auth_string.substr(colon + 1, auth_string.length() - colon - 1);
    if (username == "user" && password == "password") {
        return;
    }
}

static int digestAuth(mg_connection* conn) {
    int result = MG_FALSE;
    {
        FILE *fp;
        if ((fp = fopen("digest.txt", "w")) != nullptr) {
            fprintf(fp, "user:mydomain.com:0cf722ef3dd136b48da83758c5d855f8\n");
            fclose(fp);
        }
    }

    {
        FILE *fp;
        if ((fp = fopen("digest.txt", "r")) != nullptr) {
            result = mg_authorize_digest(conn, fp);
            fclose(fp);
        }
    }

    return result;
}

static int basicJson(mg_connection* conn) {
    auto response = std::string{"[\n"
                                "  {\n"
                                "    \"first_key\": \"first_value\",\n"
                                "    \"second_key\": \"second_value\"\n"
                                "  }\n"
                                "]"};
    mg_send_status(conn, 200);
    auto raw_header = mg_get_header(conn, "Content-type");
    std::string header;
        header = raw_header;
    }
    if (!header.empty() && header == "application/json") {
        mg_send_header(conn, "content-type", "application/json");
    } else {
        mg_send_header(conn, "content-type", "application/octet-stream");
    }
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int headerReflect(mg_connection* conn) {
    auto response = std::string{"Header reflect "} +
                    std::string{conn->request_method};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    auto num_headers = conn->num_headers;
    auto headers = conn->http_headers;
    for (int i = 0; i < num_headers; ++i) {
        auto name = headers[i].name;
        if (std::string{"Host"} != name && std::string{"Accept"} != name) {
            mg_send_header(conn, name, headers[i].value);
        }
    }
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int temporaryRedirect(mg_connection* conn) {
    auto response = std::string{"Found"};
    mg_send_status(conn, 302);
    mg_send_header(conn, "Location", "hello.html");
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int permanentRedirect(mg_connection* conn) {
    auto response = std::string{"Moved Permanently"};
    mg_send_status(conn, 301);
    mg_send_header(conn, "Location", "hello.html");
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int twoRedirects(mg_connection* conn) {
    auto response = std::string{"Moved Permanently"};
    mg_send_status(conn, 301);
    mg_send_header(conn, "Location", "permanent_redirect.html");
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int bodyGet(mg_connection* conn) {
    char message[100];
    mg_get_var(conn, "Message", message, sizeof(message));
    auto response = std::string{message};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int urlPost(mg_connection* conn) {
    mg_send_status(conn, 201);
    mg_send_header(conn, "content-type", "application/json");
    char x[100];
    char y[100];
    mg_get_var(conn, "x", x, sizeof(x));
    mg_get_var(conn, "y", y, sizeof(y));
    auto x_string = std::string{x};
    auto y_string = std::string{y};
    if (y_string.empty()) {
        auto response = std::string{
                "{\n"
                "  \"x\": " +
                x_string +
                "\n"
                "}"};
        mg_send_data(conn, response.data(), response.length());
    } else {
        std::ostringstream s;
        s << (atoi(x) + atoi(y));
        auto response = std::string{
                "{\n"
                "  \"x\": " +
                x_string +
                ",\n"
                "  \"y\": " +
                y_string +
                ",\n"
                "  \"sum\": " +
                s.str() +
                "\n"
                "}"};
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int jsonPost(mg_connection* conn) {
    auto num_headers = conn->num_headers;
    auto headers = conn->http_headers;
    auto has_json_header = false;
    for (int i = 0; i < num_headers; ++i) {
        if (std::string{"Content-Type"} == headers[i].name &&
            std::string{"application/json"} == headers[i].value) {
            has_json_header = true;
        }
    }
    if (!has_json_header) {
        auto response = std::string{"Unsupported Media Type"};
        mg_send_status(conn, 415);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_data(conn, response.data(), response.length());
        return MG_TRUE;
    }
    mg_send_status(conn, 201);
    mg_send_header(conn, "content-type", "application/json");
    auto response = std::string{conn->content, conn->content_len};
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int formPost(mg_connection* conn) {
    auto content = conn->content;
    auto content_len = conn->content_len;

    std::map<std::string, std::string> forms;

    while (true) {
        char* data = new char[10000];
        int data_len;
        char name[100];
        char filename[100];
        auto read_len =
                mg_parse_multipart(content, content_len, name, sizeof(name), filename,
                                   sizeof(filename), const_cast<const char**>(&data), &data_len);
        if (read_len == 0) {
            delete[] data;
            break;
        }

        content += read_len;
        content_len -= read_len;

        if (strlen(data) == 0) {
            delete[] data;
            break;
        }

        forms[name] = std::string{data, static_cast<unsigned long>(data_len)};
    }

    mg_send_status(conn, 201);
    mg_send_header(conn, "content-type", "application/json");
    if (forms.find("y") == forms.end()) {
        auto response = std::string{
                "{\n"
                "  \"x\": " +
                forms["x"] +
                "\n"
                "}"};
        mg_send_data(conn, response.data(), response.length());
    } else {
        std::ostringstream s;
        s << (atoi(forms["x"].data()) + atoi(forms["y"].data()));
        auto response = std::string{
                "{\n"
                "  \"x\": " +
                forms["x"] +
                ",\n"
                "  \"y\": " +
                forms["y"] +
                ",\n"
                "  \"sum\": " +
                s.str() +
                "\n"
                "}"};
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int deleteRequest(mg_connection* conn) {
    auto num_headers = conn->num_headers;
    auto headers = conn->http_headers;
    auto has_json_header = false;
    for (int i = 0; i < num_headers; ++i) {
        if (std::string{"Content-Type"} == headers[i].name &&
            std::string{"application/json"} == headers[i].value) {
            has_json_header = true;
        }
    }
    if (std::string{conn->request_method} == std::string{"DELETE"}) {
        if (!has_json_header) {
            auto response = std::string{"Delete success"};
            mg_send_status(conn, 200);
            mg_send_header(conn, "content-type", "text/html");
            mg_send_data(conn, response.data(), response.length());
        } else {
            auto response = std::string{conn->content, conn->content_len};
            mg_send_status(conn, 200);
            mg_send_header(conn, "content-type", "application/json");
            mg_send_data(conn, response.data(), response.length());
        }
    } else {
        auto response = std::string{"Method unallowed"};
        mg_send_status(conn, 405);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int deleteUnallowedRequest(mg_connection* conn) {
    if (std::string{conn->request_method} == std::string{"DELETE"}) {
        auto response = std::string{"Method unallowed"};
        mg_send_status(conn, 405);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_data(conn, response.data(), response.length());
    } else {
        auto response = std::string{"Delete success"};
        mg_send_status(conn, 200);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int patch(mg_connection* conn) {
    if (std::string{conn->request_method} == std::string{"PATCH"}) {
        mg_send_status(conn, 200);
        mg_send_header(conn, "content-type", "application/json");
        char x[100];
        char y[100];
        mg_get_var(conn, "x", x, sizeof(x));
        mg_get_var(conn, "y", y, sizeof(y));
        auto x_string = std::string{x};
        auto y_string = std::string{y};
        if (y_string.empty()) {
            auto response = std::string{
                    "{\n"
                    "  \"x\": " +
                    x_string +
                    "\n"
                    "}"};
            mg_send_data(conn, response.data(), response.length());
        } else {
            std::ostringstream s;
            s << (atoi(x) + atoi(y));
            auto response = std::string{
                    "{\n"
                    "  \"x\": " +
                    x_string +
                    ",\n"
                    "  \"y\": " +
                    y_string +
                    ",\n"
                    "  \"sum\": " +
                    s.str() +
                    "\n"
                    "}"};
            mg_send_data(conn, response.data(), response.length());
        }
    } else {
        auto response = std::string{"Method unallowed"};
        mg_send_status(conn, 405);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int patchUnallowed(mg_connection* conn) {
    if (std::string{conn->request_method} == std::string{"PATCH"}) {
        auto response = std::string{"Method unallowed"};
        mg_send_status(conn, 405);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_data(conn, response.data(), response.length());
    } else {
        auto response = std::string{"Patch success"};
        mg_send_status(conn, 200);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int put(mg_connection* conn) {
    if (std::string{conn->request_method} == std::string{"PUT"}) {
        mg_send_status(conn, 200);
        mg_send_header(conn, "content-type", "application/json");
        char x[100];
        char y[100];
        mg_get_var(conn, "x", x, sizeof(x));
        mg_get_var(conn, "y", y, sizeof(y));
        auto x_string = std::string{x};
        auto y_string = std::string{y};
        if (y_string.empty()) {
            auto response = std::string{
                    "{\n"
                    "  \"x\": " +
                    x_string +
                    "\n"
                    "}"};
            mg_send_data(conn, response.data(), response.length());
        } else {
            std::ostringstream s;
            s << (atoi(x) + atoi(y));
            auto response = std::string{
                    "{\n"
                    "  \"x\": " +
                    x_string +
                    ",\n"
                    "  \"y\": " +
                    y_string +
                    ",\n"
                    "  \"sum\": " +
                    s.str() +
                    "\n"
                    "}"};
            mg_send_data(conn, response.data(), response.length());
        }
    } else {
        auto response = std::string{"Method unallowed"};
        mg_send_status(conn, 405);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int putUnallowed(mg_connection* conn) {
    if (std::string{conn->request_method} == std::string{"PUT"}) {
        auto response = std::string{"Method unallowed"};
        mg_send_status(conn, 405);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_data(conn, response.data(), response.length());
    } else {
        auto response = std::string{"Put success"};
        mg_send_status(conn, 200);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}*/

static void evHandler(mg_connection* conn, int ev, void* ev_data) {
    switch (ev) {
        case MG_EV_HTTP_REPLY:
            /*if (Url{conn->uri} == "/basic_auth.html") {
                return basicAuth(conn);
            } else if (Url{conn->uri} == "/digest_auth.html") {
                return digestAuth(conn);
            }*/
            break;
        case MG_EV_HTTP_REQUEST:
        {
            http_message* msg = static_cast<http_message*>(ev_data);
            if (Url{msg->uri.p} == "/") {
                options(conn, msg);
            } /*else if (Url{msg->uri.p} == "/hello.html") {
                return hello(conn);
            } else if (Url{msg->uri.p} == "/timeout.html") {
                return timeout(conn);
            } else if (Url{msg->uri.p} == "/low_speed.html") {
                return lowSpeed(conn);
            } else if (Url{msg->uri.p} == "/low_speed_bytes.html") {
                return lowSpeedBytes(conn);
            } else if (Url{msg->uri.p} == "/basic_cookies.html") {
                return basicCookies(conn);
            } else if (Url{msg->uri.p} == "/check_cookies.html") {
                return checkBasicCookies(conn);
            } else if (Url{msg->uri.p} == "/v1_cookies.html") {
                return v1Cookies(conn);
            } else if (Url{msg->uri.p} == "/check_v1_cookies.html") {
                return checkV1Cookies(conn);
            } else if (Url{msg->uri.p} == "/basic_auth.html") {
                return headerReflect(conn);
            } else if (Url{msg->uri.p} == "/digest_auth.html") {
                return headerReflect(conn);
            } else if (Url{msg->uri.p} == "/basic.json") {
                return basicJson(conn);
            } else if (Url{msg->uri.p} == "/header_reflect.html") {
                return headerReflect(conn);
            } else if (Url{msg->uri.p} == "/temporary_redirect.html") {
                return temporaryRedirect(conn);
            } else if (Url{msg->uri.p} == "/permanent_redirect.html") {
                return permanentRedirect(conn);
            } else if (Url{msg->uri.p} == "/two_redirects.html") {
                return twoRedirects(conn);
            } else if (Url{msg->uri.p} == "/url_post.html") {
                return urlPost(conn);
            } else if (Url{msg->uri.p} == "/body_get.html") {
                return bodyGet(conn);
            } else if (Url{msg->uri.p} == "/json_post.html") {
                return jsonPost(conn);
            } else if (Url{msg->uri.p} == "/form_post.html") {
                return formPost(conn);
            } else if (Url{msg->uri.p} == "/delete.html") {
                return deleteRequest(conn);
            } else if (Url{msg->uri.p} == "/delete_unallowed.html") {
                return deleteUnallowedRequest(conn);
            } else if (Url{msg->uri.p} == "/put.html") {
                return put(conn);
            } else if (Url{msg->uri.p} == "/put_unallowed.html") {
                return putUnallowed(conn);
            } else if (Url{msg->uri.p} == "/patch.html") {
                return patch(conn);
            } else if (Url{msg->uri.p} == "/patch_unallowed.html") {
                return patchUnallowed(conn);
            }*/
            }
            break;
        default:
            break;
    }
}

void runServer(std::shared_ptr<mg_mgr> mgr) {
    server_cv.notify_one();
    do {
        mg_mgr_poll(mgr.get(), 1000);
    } while (!shutdown_mutex.try_lock());

    shutdown_mutex.unlock();
    std::lock_guard<std::mutex> server_lock(server_mutex);
    mg_mgr_free(mgr.get());
    server_cv.notify_one();
}

void Server::SetUp() {
    shutdown_mutex.lock();

    // Setup a new mongoose http server.
    // Based on: https://cesanta.com/docs/http/server-example.html
    std::shared_ptr<mg_mgr> mgr;
    mg_connection *c;
    mg_mgr_init(mgr.get(), nullptr);
    c = mg_bind(mgr.get(), SERVER_PORT.c_str(), evHandler);
    mg_set_protocol_http_websocket(c);

    std::unique_lock<std::mutex> server_lock(server_mutex);
    std::thread(runServer, mgr).detach();
    server_cv.wait(server_lock);
}

void Server::TearDown() {
    std::unique_lock<std::mutex> server_lock(server_mutex);
    shutdown_mutex.unlock();
    server_cv.wait(server_lock);
}

Url Server::GetBaseUrl() {
    return Url{"http://127.0.0.1:"}.append(SERVER_PORT);
}

Url Server::GetBaseUrlSSL() {
    return Url{"https://127.0.0.1:"}.append(SERVER_PORT);
}

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

/**
 * Decodes the given BASE64 string to a normal string.
 * Source: https://gist.github.com/williamdes/308b95ac9ef1ee89ae0143529c361d37
 **/
std::string base64_decode(const std::string& in) {
    std::string out;

    std::vector<int> T(256,-1);
    for (int i=0; i<64; i++) T[base64_chars[i]] = i;

    int val=0, valb=-8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val<<6) + T[c];
        valb += 6;
        if (valb>=0) {
            out.push_back(char((val>>valb)&0xFF));
            valb-=8;
        }
    }
    return out;
}

static int lowercase(const char* s) {
    return tolower(*reinterpret_cast<const unsigned char*>(s));
}

static int mg_strncasecmp(const char* s1, const char* s2, size_t len) {
    int diff = 0;

    if (len > 0) {
        do {
            diff = lowercase(s1++) - lowercase(s2++);
        } while (diff == 0 && s1[-1] != '\0' && --len > 0);
    }

    return diff;
}
