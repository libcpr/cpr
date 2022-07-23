#include "httpsServer.hpp"
#include <system_error>

namespace cpr {
HttpsServer::HttpsServer(fs::path&& baseDirPath, fs::path&& sslCertFileName, fs::path&& sslKeyFileName) : baseDirPath(baseDirPath.make_preferred().string()), sslCertFileName(sslCertFileName.make_preferred().string()), sslKeyFileName(sslKeyFileName.make_preferred().string()) {
    // See https://mongoose.ws/tutorials/tls/
    memset(static_cast<void*>(&tlsOpts), 0, sizeof(tlsOpts));
    tlsOpts.cert = this->sslCertFileName.c_str();
    tlsOpts.certkey = this->sslKeyFileName.c_str();
}

std::string HttpsServer::GetBaseUrl() {
    return "https://127.0.0.1:" + std::to_string(GetPort());
}

uint16_t HttpsServer::GetPort() {
    // Unassigned port in the ephemeral range
    return 61937;
}

mg_connection* HttpsServer::initServer(mg_mgr* mgr, mg_event_handler_t event_handler) {
    mg_mgr_init(mgr);

    std::string port = std::to_string(GetPort());
    mg_connection* c = mg_http_listen(mgr, GetBaseUrl().c_str(), event_handler, this);
    return c;
}

void HttpsServer::acceptConnection(mg_connection* conn) {
    // See https://mongoose.ws/tutorials/tls/
    mg_tls_init(conn, &tlsOpts);
}

void HttpsServer::OnRequest(mg_connection* conn, mg_http_message* msg) {
    std::string uri = std::string(msg->uri.ptr, msg->uri.len);
    if (uri == "/hello.html") {
        OnRequestHello(conn, msg);
    } else {
        OnRequestNotFound(conn, msg);
    }
}

void HttpsServer::OnRequestNotFound(mg_connection* conn, mg_http_message* /*msg*/) {
    mg_http_reply(conn, 404, nullptr, "Not Found");
}

void HttpsServer::OnRequestHello(mg_connection* conn, mg_http_message* /*msg*/) {
    std::string response{"Hello world!"};
    std::string headers{"Content-Type: text/html\r\n"};
    mg_http_reply(conn, 200, headers.c_str(), response.c_str());
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
