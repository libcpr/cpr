#include "cpr/connection_pool.h"
#include <iostream>

namespace cpr {
ConnectionPool::ConnectionPool() {
    auto curl_share = curl_share_init();
    this->connection_mutex_ = std::make_shared<std::mutex>();
    // Force mutex initalization
    this->connection_mutex_->lock();
    this->connection_mutex_->unlock();
    auto lock_f = +[](CURL* handle, curl_lock_data data, curl_lock_access access, void* userptr) {
        std::mutex* lock = static_cast<std::mutex*>(userptr);
        lock->lock();
    };
    auto unlock_f = +[](CURL* handle, curl_lock_data data, void* userptr) {
        std::mutex* lock = static_cast<std::mutex*>(userptr);
        lock->unlock();
    };
    curl_share_setopt(curl_share, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
    curl_share_setopt(curl_share, CURLSHOPT_USERDATA, this->connection_mutex_.get());
    curl_share_setopt(curl_share, CURLSHOPT_LOCKFUNC, lock_f);
    curl_share_setopt(curl_share, CURLSHOPT_UNLOCKFUNC, unlock_f);
    this->curl_sh_ =
            std::shared_ptr<CURLSH>(curl_share, [](CURLSH* ptr) { curl_share_cleanup(ptr); });
}

void ConnectionPool::SetupHandler(CURL* easy_handler) const {
    curl_easy_setopt(easy_handler, CURLOPT_SHARE, this->curl_sh_.get());
}

} // namespace cpr
