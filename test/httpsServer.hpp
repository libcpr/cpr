#ifndef CPR_TEST_HTTPS_SERVER_H
#define CPR_TEST_HTTPS_SERVER_H

#include <memory>
#include <string>

#include "abstractServer.hpp"
#include "cpr/cpr.h"
#include "cpr/filesystem.h"
#include "mongoose.h"

namespace cpr {
class HttpsServer : public AbstractServer {
  private:
    // We don't use fs::path here, as this leads to problems using windows
    const std::string baseDirPath;
    const std::string sslCertFileName;
    const std::string sslKeyFileName;
    struct mg_tls_opts tlsOpts;

  public:
    explicit HttpsServer(fs::path&& baseDirPath, fs::path&& sslCertFileName, fs::path&& sslKeyFileName);
    ~HttpsServer() override = default;

    std::string GetBaseUrl() override;
    uint16_t GetPort() override;

    void OnRequest(mg_connection* conn, mg_http_message* msg) override;
    static void OnRequestHello(mg_connection* conn, mg_http_message* msg);
    static void OnRequestNotFound(mg_connection* conn, mg_http_message* msg);

    const std::string& getBaseDirPath() const;
    const std::string& getSslCertFileName() const;
    const std::string& getSslKeyFileName() const;

  protected:
    mg_connection* initServer(mg_mgr* mgr, mg_event_handler_t event_handler) override;
    void acceptConnection(mg_connection* conn) override;
};
} // namespace cpr

#endif
