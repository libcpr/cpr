#ifndef CPR_TEST_ABSTRACT_SERVER_SERVER_H
#define CPR_TEST_ABSTRACT_SERVER_SERVER_H

#include <atomic>
#include <condition_variable>
#include <gtest/gtest.h>
#include <memory>
#include <mutex>
#include <string>

#include "cpr/cpr.h"
#include "mongoose.h"

namespace cpr {
class AbstractServer : public testing::Environment {
  public:
    ~AbstractServer() override = default;

    void SetUp() override;
    void TearDown() override;

    void Start();
    void Stop();

    virtual std::string GetBaseUrl() = 0;
    virtual uint16_t GetPort() = 0;

    virtual void acceptConnection(mg_connection* conn) = 0;
    virtual void OnRequest(mg_connection* conn, mg_http_message* msg) = 0;

  private:
    std::shared_ptr<std::thread> serverThread{nullptr};
    std::mutex server_mutex;
    std::condition_variable server_start_cv;
    std::condition_variable server_stop_cv;
    std::atomic<bool> should_run{false};

    void Run();

  protected:
    mg_mgr mgr{};
    virtual mg_connection* initServer(mg_mgr* mgr, mg_event_handler_t event_handler) = 0;

    static std::string Base64Decode(const std::string& in);
    static void SendError(mg_connection* conn, int code, std::string& reason);
};
} // namespace cpr

#endif
