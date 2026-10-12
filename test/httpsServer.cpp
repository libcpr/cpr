#include "httpsServer.hpp"
#include <fstream>
#include <ios>
#include <iterator>
#include <stdexcept>
#include <string>

#include "cpr/filesystem.h" // IWYU pragma: keep
#include "mongoose.h"

// NOLINTNEXTLINE(misc-use-anonymous-namespace) LLVM prefers static functions.
static std::string LoadTlsFile(const std::string& path) {
    std::ifstream file{path, std::ios::binary};
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open TLS file: " + path);
    }
    std::string content{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
    if (file.bad() || content.empty()) {
        throw std::runtime_error("Failed to read TLS file: " + path);
    }
    return content;
}

namespace cpr {
HttpsServer::HttpsServer(fs::path&& baseDirPath, fs::path&& sslCertFileName, fs::path&& sslKeyFileName) : baseDirPath(baseDirPath.make_preferred().string()), sslCertFileName(sslCertFileName.make_preferred().string()), sslKeyFileName(sslKeyFileName.make_preferred().string()), sslCert(LoadTlsFile(this->sslCertFileName)), sslKey(LoadTlsFile(this->sslKeyFileName)) {
    // See https://mongoose.ws/tutorials/tls/
    memset(static_cast<void*>(&tlsOpts), 0, sizeof(tlsOpts));
    tlsOpts.cert = mg_str(sslCert.c_str());
    tlsOpts.key = mg_str(sslKey.c_str());
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

    mg_connection* c = mg_http_listen(mgr, GetBaseUrl().c_str(), event_handler, this);
    return c;
}

void HttpsServer::acceptConnection(mg_connection* conn) {
    // See https://mongoose.ws/tutorials/tls/
    mg_tls_init(conn, &tlsOpts);
}

void HttpsServer::OnRequest(mg_connection* conn, mg_http_message* msg) {
    std::string uri = std::string(msg->uri.buf, msg->uri.len);
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
