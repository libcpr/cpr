#include "server.h"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "mongoose.h"

#define SERVER_PORT "8080"


std::mutex shutdown_mutex;
std::mutex start_mutex;
std::mutex server_mutex;
std::condition_variable start_cv;

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789+/";

static inline bool is_base64(unsigned char c);
std::string base64_decode(std::string const& encoded_string);
static int lowercase(const char *s);
static int mg_strncasecmp(const char *s1, const char *s2, size_t len);

static int hello(struct mg_connection* conn) {
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
    std::string auth_string;
    if ((requested_auth = mg_get_header(conn, "Authorization")) == NULL ||
        mg_strncasecmp(requested_auth, auth.data(), auth.length()) != 0) {
        return MG_FALSE;
    }
    auth_string = {requested_auth};
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
    auto response = std::string{"Header reflect"};
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

static int evHandler(struct mg_connection* conn, enum mg_event ev) {
    switch (ev) {
        case MG_AUTH:
            if (Url{conn->uri} == "/basic_auth.html") {
                return basicAuth(conn);
            }
            return MG_TRUE;
        case MG_REQUEST:
            if (Url{conn->uri} == "/hello.html") {
                return hello(conn);
            } else if (Url{conn->uri} == "/basic_auth.html") {
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
            }
            return MG_FALSE;
        default:
            return MG_FALSE;
    }
}

void runServer(struct mg_server* server) {
    std::unique_lock<std::mutex> server_lock(server_mutex);
    {
        std::unique_lock<std::mutex> start_lock(start_mutex);
        mg_set_option(server, "listening_port", SERVER_PORT);
        start_cv.notify_one();
    }
    do {
        mg_poll_server(server, 1000);
    } while (!shutdown_mutex.try_lock());
    mg_destroy_server(&server);
}

void Server::SetUp() {
    shutdown_mutex.lock();
    struct mg_server* server;
    server = mg_create_server(NULL, evHandler);
    std::unique_lock<std::mutex> start_lock(start_mutex);
    std::thread(runServer, server).detach();
    start_cv.wait(start_lock);
}

void Server::TearDown() {
    shutdown_mutex.unlock();
    std::unique_lock<std::mutex> server_lock(server_mutex);
}

Url Server::GetBaseUrl() {
    return Url{"http://127.0.0.1:"}.append(SERVER_PORT);
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

