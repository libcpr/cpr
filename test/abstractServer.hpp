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

// Helper struct for functions using timers to simulate slow connections
struct TimerArg {
    mg_mgr* mgr;
    mg_connection* connection;
    unsigned long connection_id;
    mg_timer timer;
    unsigned counter;

    explicit TimerArg(mg_mgr* m, mg_connection* c, mg_timer&& t) : mgr{m}, connection{c}, connection_id{0}, timer{t}, counter{0} {}

    ~TimerArg() {
        mg_timer_free(&mgr->timers, &timer);
    }
};

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
    std::vector<std::unique_ptr<TimerArg>> timer_args{};
    virtual mg_connection* initServer(mg_mgr* mgr, mg_event_handler_t event_handler) = 0;

    static std::string Base64Decode(const std::string& in);
    static void SendError(mg_connection* conn, int code, std::string& reason);
    static bool IsConnectionActive(mg_mgr* mgr, mg_connection* conn);

    static uint16_t GetRemotePort(const mg_connection* conn);
    static uint16_t GetLocalPort(const mg_connection* conn);
};
} // namespace cpr

#endif
