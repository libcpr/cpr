#include "httpsServer.hpp"
#include <system_error>

namespace cpr {
HttpsServer::HttpsServer(std::string&& baseDirPath, std::string&& sslCertFileName,
                         std::string&& sslKeyFileName)
        : baseDirPath(std::move(baseDirPath)), sslCertFileName(std::move(sslCertFileName)),
          sslKeyFileName(std::move(sslKeyFileName)) {}

std::string HttpsServer::GetBaseUrl() {
    return "https://127.0.0.1:" + std::to_string(GetPort());
}

uint16_t HttpsServer::GetPort() {
    // Unassigned port in the ephemeral range
    return 61937;
}

mg_connection* HttpsServer::initServer(mg_mgr* mgr,
                                       MG_CB(mg_event_handler_t event_handler, void* user_data)) {
    // https://cesanta.com/docs/http/ssl.html
    mg_mgr_init(mgr, this);

    mg_bind_opts bind_opts{};
    bind_opts.ssl_cert = sslCertFileName.c_str();
    bind_opts.ssl_key = sslKeyFileName.c_str();
    std::string port = std::to_string(GetPort());
    mg_connection* c = mg_bind_opt(mgr, port.c_str(), event_handler, bind_opts);
    if (!c) {
        throw std::system_error(errno, std::system_category(), "Failed to bind to port " + port);
    }
    mg_set_protocol_http_websocket(c);
    return c;
}

void HttpsServer::OnRequest(mg_connection* conn, http_message* msg) {
    std::string uri = std::string(msg->uri.p, msg->uri.len);
    if (uri == "/hello.html") {
        OnRequestHello(conn, msg);
    } else {
        OnRequestNotFound(conn, msg);
    }
}

void HttpsServer::OnRequestNotFound(mg_connection* conn, http_message*  /*msg*/) {
    mg_http_send_error(conn, 404, "Not Found");
}

void HttpsServer::OnRequestHello(mg_connection* conn, http_message*  /*msg*/) {
    std::string response{"Hello world!"};
    std::string headers = "Content-Type: text/html";
    mg_send_head(conn, 200, response.length(), headers.c_str());
    mg_send(conn, response.c_str(), response.length());
}

const std::string& HttpsServer::getBaseDirPath() const {
    return baseDirPath;
}

const std::string& HttpsServer::getSslCertFileName() const {
    return sslCertFileName;
}

const std::string& HttpsServer::getSslKeyFileName() const {
    return sslKeyFileName;
}

} // namespace cpr
