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

    virtual void OnRequest(mg_connection* conn, http_message* msg) = 0;

  private:
    std::shared_ptr<std::thread> serverThread{nullptr};
    std::mutex server_mutex;
    std::condition_variable server_start_cv;
    std::condition_variable server_stop_cv;
    std::atomic<bool> should_run{false};
    mg_mgr mgr{};

    void Run();

  protected:
    virtual mg_connection* initServer(mg_mgr* mgr, MG_CB(mg_event_handler_t event_handler, void* user_data)) = 0;

    static std::string Base64Decode(const std::string& in);
};
} // namespace cpr

#endif
