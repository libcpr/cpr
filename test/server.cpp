#include "server.h"

#include <thread>
#include <iostream>

#include "mongoose.h"

#include "cprtypes.h"


std::mutex shutdown_mutex;
std::mutex server_mutex;

static int hello(struct mg_connection* conn) {
    auto response = std::string{"Hello world!"};
    mg_send_status(conn, 200);
    mg_send_header(conn, "content-type", "text/html");
    mg_send_data(conn, response.data(), response.length()); 
    return MG_TRUE;
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

static int evHandler(struct mg_connection* conn, enum mg_event ev) {
    switch (ev) {
        case MG_AUTH:
            return MG_TRUE;
        case MG_REQUEST:
            if (Url{conn->uri} == "/hello.html") {
                return hello(conn);
            }
            if (Url{conn->uri} == "/basic.json") {
                return basicJson(conn);
            }
            return MG_FALSE;
        default:
            return MG_FALSE;
    }
}

void runServer() {
    server_mutex.lock();
    struct mg_server* server;
    server = mg_create_server(NULL, evHandler);
    mg_set_option(server, "listening_port", "8080");
    while (true) {
        if (!shutdown_mutex.try_lock()) {
            mg_poll_server(server, 1000);
        } else {
            shutdown_mutex.unlock();
            mg_destroy_server(&server);
            server_mutex.unlock();
            break;
        }
    }
}

void Server::SetUp() {
    shutdown_mutex.lock();
    std::thread(runServer).detach();
}

void Server::TearDown() {
    shutdown_mutex.unlock();
    server_mutex.lock();
    server_mutex.unlock();
}
