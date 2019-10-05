#ifndef CPR_CONNECTION_POOL_H
#define CPR_CONNECTION_POOL_H

#include <curl/curl.h>
#include <mutex>
#include <memory>

namespace cpr {
class ConnectionPool {
  public:
    ConnectionPool();
    ConnectionPool(const ConnectionPool&) = default;
    ConnectionPool & operator=(const ConnectionPool&) = delete;
    void SetupHandler(CURL* easy_handler) const;

  private:
    std::shared_ptr<CURLSH> curl_sh_;
    std::shared_ptr<std::mutex> connection_mutex_;
};
} // namespace cpr
#endif
