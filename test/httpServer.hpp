#ifndef CPR_TEST_HTTP_SERVER_H
#define CPR_TEST_HTTP_SERVER_H

#include <condition_variable>
#include <gtest/gtest.h>
#include <memory>
#include <mutex>
#include <string>

#include "cpr/cpr.h"
#include "mongoose/mongoose.h"

namespace cpr {
class HttpServer : public testing::Environment {
  public:
    virtual ~HttpServer() = default;

    virtual void SetUp();
    virtual void TearDown();

    Url GetBaseUrl();
    Url GetBaseUrlSSL();
    uint16_t GetPort();

    void Start();
    void Stop();

    void OnRequest(mg_connection* conn, http_message* msg);
    void OnChunk(mg_connection* conn, http_message* msg);

  private:
    std::shared_ptr<std::thread> serverThread{nullptr};
    std::mutex server_mutex;
    std::condition_variable server_start_cv;
    std::condition_variable server_stop_cv;
    bool should_run{false};

    void Run();

    static void OnRequestHello(mg_connection* conn, http_message* msg);
    static void OnRequestRoot(mg_connection* conn, http_message* msg);
    static void OnRequestOptions(mg_connection* conn, http_message* msg);
    static void OnRequestNotFound(mg_connection* conn, http_message* msg);
    static void OnRequestTimeout(mg_connection* conn, http_message* msg);
    static void OnRequestLowSpeed(mg_connection* conn, http_message* msg);
    static void OnRequestLowSpeedBytes(mg_connection* conn, http_message* msg);
    static void OnRequestBasicCookies(mg_connection* conn, http_message* msg);
    static void OnRequestCheckCookies(mg_connection* conn, http_message* msg);
    static void OnRequestV1Cookies(mg_connection* conn, http_message* msg);
    static void OnRequestCheckV1Cookies(mg_connection* conn, http_message* msg);
    static void OnRequestBasicAuth(mg_connection* conn, http_message* msg);
    static void OnRequestDigestAuth(mg_connection* conn, http_message* msg);
    static void OnRequestBasicJson(mg_connection* conn, http_message* msg);
    static void OnRequestHeaderReflect(mg_connection* conn, http_message* msg);
    static void OnRequestTempRedirect(mg_connection* conn, http_message* msg);
    static void OnRequestPermRedirect(mg_connection* conn, http_message* msg);
    static void OnRequestTwoRedirects(mg_connection* conn, http_message* msg);
    static void OnRequestUrlPost(mg_connection* conn, http_message* msg);
    static void OnRequestBodyGet(mg_connection* conn, http_message* msg);
    static void OnRequestJsonPost(mg_connection* conn, http_message* msg);
    static void OnRequestFormPost(mg_connection* conn, http_message* msg);
    static void OnRequestDelete(mg_connection* conn, http_message* msg);
    static void OnRequestDeleteNotAllowed(mg_connection* conn, http_message* msg);
    static void OnRequestPut(mg_connection* conn, http_message* msg);
    static void OnRequestPutNotAllowed(mg_connection* conn, http_message* msg);
    static void OnRequestPatch(mg_connection* conn, http_message* msg);
    static void OnRequestPatchNotAllowed(mg_connection* conn, http_message* msg);

    static std::string Base64Decode(const std::string& in);
    static int LowerCase(const char* s);
    static int StrnCaseCmp(const char* s1, const char* s2, size_t len);
};
} // namespace cpr

#endif