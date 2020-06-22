#ifndef CPR_TEST_HTTP_SERVER_H
#define CPR_TEST_HTTP_SERVER_H

#include <memory>
#include <string>

#include "abstractServer.hpp"
#include "cpr/cpr.h"
#include "mongoose.h"

namespace cpr {
class HttpServer : public AbstractServer {
  public:
    ~HttpServer() = default;

    std::string GetBaseUrl() override;
    uint16_t GetPort() override;

    void OnRequest(mg_connection* conn, http_message* msg) override;

  private:
    static void OnRequestHello(mg_connection* conn, http_message* msg);
    static void OnRequestRoot(mg_connection* conn, http_message* msg);
    static void OnRequestOptions(mg_connection* conn, http_message* msg);
    static void OnRequestNotFound(mg_connection* conn, http_message* msg);
    static void OnRequestTimeout(mg_connection* conn, http_message* msg);
    static void OnRequestLowSpeed(mg_connection* conn, http_message* msg);
    static void OnRequestLowSpeedBytes(mg_connection* conn, http_message* msg);
    static void OnRequestBasicCookies(mg_connection* conn, http_message* msg);
    static void OnRequestEmptyCookies(mg_connection* conn, http_message* msg);
    static void OnRequestCheckCookies(mg_connection* conn, http_message* msg);
    static void OnRequestV1Cookies(mg_connection* conn, http_message* msg);
    static void OnRequestCheckV1Cookies(mg_connection* conn, http_message* msg);
    static void OnRequestBasicAuth(mg_connection* conn, http_message* msg);
    static void OnRequestBasicJson(mg_connection* conn, http_message* msg);
    static void OnRequestHeaderReflect(mg_connection* conn, http_message* msg);
    static void OnRequestTempRedirect(mg_connection* conn, http_message* msg);
    static void OnRequestPermRedirect(mg_connection* conn, http_message* msg);
    static void OnRequestTwoRedirects(mg_connection* conn, http_message* msg);
    static void OnRequestUrlPost(mg_connection* conn, http_message* msg);
    static void OnRequestReflectPost(mg_connection* conn, http_message* msg);
    static void OnRequestBodyGet(mg_connection* conn, http_message* msg);
    static void OnRequestJsonPost(mg_connection* conn, http_message* msg);
    static void OnRequestFormPost(mg_connection* conn, http_message* msg);
    static void OnRequestDelete(mg_connection* conn, http_message* msg);
    static void OnRequestDeleteNotAllowed(mg_connection* conn, http_message* msg);
    static void OnRequestPut(mg_connection* conn, http_message* msg);
    static void OnRequestPutNotAllowed(mg_connection* conn, http_message* msg);
    static void OnRequestPatch(mg_connection* conn, http_message* msg);
    static void OnRequestPatchNotAllowed(mg_connection* conn, http_message* msg);

  protected:
    mg_connection* initServer(mg_mgr* mgr,
                              MG_CB(mg_event_handler_t event_handler, void* user_data)) override;
};
} // namespace cpr

#endif
