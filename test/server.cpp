#include "server.h"

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <sstream>
#include <thread>

#include "mongoose.h"

#include <time.h>

#define SERVER_PORT "8080"


std::mutex shutdown_mutex;
std::mutex server_mutex;
std::condition_variable server_cv;

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789+/";

static inline bool is_base64(unsigned char c);
std::string base64_decode(std::string const& encoded_string);
static int lowercase(const char *s);
static int mg_strncasecmp(const char *s1, const char *s2, size_t len);

static int options(struct mg_connection* conn) {
    if (std::string{conn->request_method} == std::string{"OPTIONS"}) {
        auto response = std::string{""};
        mg_send_status(conn, 200);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_header(conn, "Access-Control-Allow-Origin", "*");
        mg_send_header(conn, "Access-Control-Allow-Credentials", "true");
        mg_send_header(conn, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, PATCH, OPTIONS");
        mg_send_header(conn, "Access-Control-Max-Age", "3600");
        mg_send_data(conn, response.data(), response.length());
    } else {
        auto response = std::string{"Method unallowed"};
        mg_send_status(conn, 405);
        mg_send_header(conn, "content-type", "text/html");
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int hello(struct mg_connection* conn) {
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

static int timeout(struct mg_connection* conn) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto response = std::string{"Hello world!"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int lowSpeed(struct mg_connection* conn) {
    auto response = std::string{"Hello world!"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int lowSpeedBytes(struct mg_connection* conn) {
    auto response = std::string{"a"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    for (auto i = 0; i < 20; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int basicCookies(struct mg_connection* conn) {
    auto response = std::string{"Hello world!"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    time_t t = time(NULL) + 5;  // Valid for 1 hour
    char expire[100], expire_epoch[100];
    snprintf(expire_epoch, sizeof(expire_epoch), "%lu", (unsigned long) t);
    strftime(expire, sizeof(expire), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
    std::string cookie{"cookie=chocolate; expires=\"" + std::string{expire} + "\"; http-only;"};
    std::string cookie2{"icecream=vanilla; expires=\"" + std::string{expire} + "\"; http-only;"};
    mg_send_header(conn, "Set-Cookie", cookie.data());
    mg_send_header(conn, "Set-Cookie", cookie2.data());
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int v1Cookies(struct mg_connection* conn) {
    auto response = std::string{"Hello world!"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    time_t t = time(NULL) + 5; // Valid for 1 hour
    char expire[100], expire_epoch[100];
    snprintf(expire_epoch, sizeof(expire_epoch), "%lu", (unsigned long) t);
    strftime(expire, sizeof(expire), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
    std::string v1cookie{"cookie=\"value with spaces (v1 cookie)\"; expires=\"" +
                         std::string{expire} + "\"; http-only;"};
    mg_send_header(conn, "Set-Cookie", v1cookie.data());
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int checkBasicCookies(struct mg_connection* conn) {
    const char* request_cookies;
    if ((request_cookies = mg_get_header(conn, "Cookie")) == NULL)
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

static int checkV1Cookies(struct mg_connection* conn) {
    const char* request_cookies;
    if ((request_cookies = mg_get_header(conn, "Cookie")) == NULL)
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

static int basicAuth(struct mg_connection* conn) {
    auto response = std::string{"Hello world!"};
    const char* requested_auth;
    auto auth = std::string{"Basic"};
    if ((requested_auth = mg_get_header(conn, "Authorization")) == NULL ||
        mg_strncasecmp(requested_auth, auth.data(), auth.length()) != 0) {
        return MG_FALSE;
    }
    auto auth_string = std::string{requested_auth};
    auto basic_token = auth_string.find(' ') + 1;
    auth_string = auth_string.substr(basic_token, auth_string.length() - basic_token);
    auth_string = base64_decode(auth_string);
    auto colon = auth_string.find(':');
    auto username = auth_string.substr(0, colon);
    auto password = auth_string.substr(colon + 1, auth_string.length() - colon - 1);
    if (username == "user" && password == "password") {
        return MG_TRUE;
    }

    return MG_FALSE;
}

static int digestAuth(struct mg_connection* conn) {
    int result = MG_FALSE;
    {
        FILE *fp;
        if ((fp = fopen("digest.txt", "w")) != NULL) {
            fprintf(fp, "user:mydomain.com:0cf722ef3dd136b48da83758c5d855f8\n");
            fclose(fp);
        }
    }

    {
        FILE *fp;
        if ((fp = fopen("digest.txt", "r")) != NULL) {
            result = mg_authorize_digest(conn, fp);
            fclose(fp);
        }
    }

    return result;
}

static int basicJson(struct mg_connection* conn) {
    auto response = std::string{"[\n"
                                "  {\n"
                                "    \"first_key\": \"first_value\",\n"
                                "    \"second_key\": \"second_value\"\n"
                                "  }\n"
                                "]"};
    mg_send_status(conn, 200);
    auto raw_header = mg_get_header(conn, "Content-type");
    std::string header;
    if (raw_header != NULL) {
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

static int headerReflect(struct mg_connection* conn) {
    auto response = std::string{"Header reflect "} +
                    std::string{conn->request_method};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    auto num_headers = conn->num_headers;
    auto headers = conn->http_headers;
    for (int i = 0; i < num_headers; ++i) {
        auto name = headers[i].name;
        if (std::string{"User-Agent"} != name &&
                std::string{"Host"} != name &&
                std::string{"Accept"} != name) {
            mg_send_header(conn, name, headers[i].value);
        }
    }
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int temporaryRedirect(struct mg_connection* conn) {
    auto response = std::string{"Found"};
    mg_send_status(conn, 302);
    mg_send_header(conn, "Location", "hello.html");
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int permanentRedirect(struct mg_connection* conn) {
    auto response = std::string{"Moved Permanently"};
    mg_send_status(conn, 301);
    mg_send_header(conn, "Location", "hello.html");
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int twoRedirects(struct mg_connection* conn) {
    auto response = std::string{"Moved Permanently"};
    mg_send_status(conn, 301);
    mg_send_header(conn, "Location", "permanent_redirect.html");
    mg_send_data(conn, response.data(), response.length());
    return MG_TRUE;
}

static int urlPost(struct mg_connection* conn) {
    mg_send_status(conn, 201);
    mg_send_header(conn, "content-type", "application/json");
    char x[100];
    char y[100];
    mg_get_var(conn, "x", x, sizeof(x));
    mg_get_var(conn, "y", y, sizeof(y));
    auto x_string = std::string{x};
    auto y_string = std::string{y};
    if (y_string.empty()) {
        auto response = std::string{"{\n"
                                    "  \"x\": " + x_string + "\n"
                                    "}"};
        mg_send_data(conn, response.data(), response.length());
    } else {
        std::ostringstream s;
        s << (atoi(x) + atoi(y));
        auto response = std::string{"{\n"
                                    "  \"x\": " + x_string + ",\n"
                                    "  \"y\": " + y_string + ",\n"
                                    "  \"sum\": " + s.str() + "\n"
                                    "}"};
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int jsonPost(struct mg_connection* conn) {
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

static int formPost(struct mg_connection* conn) {
    auto content = conn->content;
    auto content_len = conn->content_len;

    std::map<std::string, std::string> forms;

    while (true) {
        char* data = new char[10000];
        int data_len;
        char name[100];
        char filename[100];
        content += mg_parse_multipart(content, content_len,
                                      name, sizeof(name),
                                      filename, sizeof(filename),
                                      (const char**) &data, &data_len);
        if (strlen(data) == 0) {
            delete[] data;
            break;
        }

        forms[name] = std::string{data, (unsigned long) data_len};
    }

    mg_send_status(conn, 201);
    mg_send_header(conn, "content-type", "application/json");
    if (forms.find("y") == forms.end()) {
        auto response = std::string{"{\n"
                                    "  \"x\": " + forms["x"] + "\n"
                                    "}"};
        mg_send_data(conn, response.data(), response.length());
    } else {
        std::ostringstream s;
        s << (atoi(forms["x"].data()) + atoi(forms["y"].data()));
        auto response = std::string{"{\n"
                                    "  \"x\": " + forms["x"] + ",\n"
                                    "  \"y\": " + forms["y"] + ",\n"
                                    "  \"sum\": " + s.str() + "\n"
                                    "}"};
        mg_send_data(conn, response.data(), response.length());
    }
    return MG_TRUE;
}

static int deleteRequest(struct mg_connection* conn) {
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

static int deleteUnallowedRequest(struct mg_connection* conn) {
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

static int patch(struct mg_connection* conn) {
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
            auto response = std::string{"{\n"
                                        "  \"x\": " + x_string + "\n"
                                        "}"};
            mg_send_data(conn, response.data(), response.length());
        } else {
            std::ostringstream s;
            s << (atoi(x) + atoi(y));
            auto response = std::string{"{\n"
                                        "  \"x\": " + x_string + ",\n"
                                        "  \"y\": " + y_string + ",\n"
                                        "  \"sum\": " + s.str() + "\n"
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

static int patchUnallowed(struct mg_connection* conn) {
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

static int put(struct mg_connection* conn) {
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
            auto response = std::string{"{\n"
                                        "  \"x\": " + x_string + "\n"
                                        "}"};
            mg_send_data(conn, response.data(), response.length());
        } else {
            std::ostringstream s;
            s << (atoi(x) + atoi(y));
            auto response = std::string{"{\n"
                                        "  \"x\": " + x_string + ",\n"
                                        "  \"y\": " + y_string + ",\n"
                                        "  \"sum\": " + s.str() + "\n"
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

static int putUnallowed(struct mg_connection* conn) {
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
}

static int evHandler(struct mg_connection* conn, enum mg_event ev) {
    switch (ev) {
        case MG_AUTH:
            if (Url{conn->uri} == "/basic_auth.html") {
                return basicAuth(conn);
            } else if (Url{conn->uri} == "/digest_auth.html") {
                return digestAuth(conn);
            }
            return MG_TRUE;
        case MG_REQUEST:
            if (Url{conn->uri} == "/") {
                return options(conn);
            } else if (Url{conn->uri} == "/hello.html") {
                return hello(conn);
            } else if (Url{conn->uri} == "/timeout.html") {
                return timeout(conn);
            } else if (Url{conn->uri} == "/low_speed.html") {
                return lowSpeed(conn);				
            } else if (Url{conn->uri} == "/low_speed_bytes.html") {
                return lowSpeedBytes(conn);				
            } else if (Url{conn->uri} == "/basic_cookies.html") {
                return basicCookies(conn);
            } else if (Url{conn->uri} == "/check_cookies.html") {
                return checkBasicCookies(conn);
            } else if (Url{conn->uri} == "/v1_cookies.html") {
                return v1Cookies(conn);
            } else if (Url{conn->uri} == "/check_v1_cookies.html") {
                return checkV1Cookies(conn);
            } else if (Url{conn->uri} == "/basic_auth.html") {
                return headerReflect(conn);
            } else if (Url{conn->uri} == "/digest_auth.html") {
                return headerReflect(conn);
            } else if (Url{conn->uri} == "/basic.json") {
                return basicJson(conn);
            } else if (Url{conn->uri} == "/header_reflect.html") {
                return headerReflect(conn);
            } else if (Url{conn->uri} == "/temporary_redirect.html") {
                return temporaryRedirect(conn);
            } else if (Url{conn->uri} == "/permanent_redirect.html") {
                return permanentRedirect(conn);
            } else if (Url{conn->uri} == "/two_redirects.html") {
                return twoRedirects(conn);
            } else if (Url{conn->uri} == "/url_post.html") {
                return urlPost(conn);
            } else if (Url{conn->uri} == "/json_post.html") {
                return jsonPost(conn);
            } else if (Url{conn->uri} == "/form_post.html") {
                return formPost(conn);
            } else if (Url{conn->uri} == "/delete.html") {
                return deleteRequest(conn);
            } else if (Url{conn->uri} == "/delete_unallowed.html") {
                return deleteUnallowedRequest(conn);
            } else if (Url{conn->uri} == "/put.html") {
                return put(conn);
            } else if (Url{conn->uri} == "/put_unallowed.html") {
                return putUnallowed(conn);
            } else if (Url{conn->uri} == "/patch.html") {
                return patch(conn);
            } else if (Url{conn->uri} == "/patch_unallowed.html") {
                return patchUnallowed(conn);
            }
            return MG_FALSE;
        default:
            return MG_FALSE;
    }
}

void runServer(struct mg_server* server) {
    {
        std::lock_guard<std::mutex> server_lock(server_mutex);
        mg_set_option(server, "listening_port", SERVER_PORT);
        server_cv.notify_one();
    }

    do {
        mg_poll_server(server, 1000);
    } while (!shutdown_mutex.try_lock());

    std::lock_guard<std::mutex> server_lock(server_mutex);
    mg_destroy_server(&server);
    server_cv.notify_one();
}

void Server::SetUp() {
    shutdown_mutex.lock();
    struct mg_server* server;
    server = mg_create_server(NULL, evHandler);
    std::unique_lock<std::mutex> server_lock(server_mutex);
    std::thread(runServer, server).detach();
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

std::string base64_decode(std::string const& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++) {
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++) {
                ret += char_array_3[i];
            }

            i = 0;
        }
    }

    if (i) {
        for (j = i; j <4; j++) {
            char_array_4[j] = 0;
        }

        for (j = 0; j <4; j++) {
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) {
            ret += char_array_3[j];
        }
    }

    return ret;
}

static int lowercase(const char *s) {
    return tolower(* (const unsigned char *) s);
}

static int mg_strncasecmp(const char *s1, const char *s2, size_t len) {
    int diff = 0;

    if (len > 0) {
        do {
            diff = lowercase(s1++) - lowercase(s2++);
        } while (diff == 0 && s1[-1] != '\0' && --len > 0);
    }

    return diff;
}

