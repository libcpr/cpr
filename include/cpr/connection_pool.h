#ifndef CPR_CONNECTION_POOL_H
#define CPR_CONNECTION_POOL_H

#include <curl/curl.h>
#include <memory>
#include <mutex>

namespace cpr {
/**
 * cpr connection pool implementation for sharing connections between HTTP requests.
 *
 * The ConnectionPool enables connection reuse across multiple HTTP requests to the same host,
 * which can significantly improve performance by avoiding the overhead of establishing new
 * connections for each request. It uses libcurl's CURLSH (share) interface to manage
 * connection sharing in a thread-safe manner.
 *
 * Example:
 * ```cpp
 * // Create a connection pool
 * cpr::ConnectionPool pool;
 * 
 * // Use the pool with requests to reuse connections
 * cpr::Response r1 = cpr::Get(cpr::Url{"http://example.com/api/data"}, pool);
 * cpr::Response r2 = cpr::Get(cpr::Url{"http://example.com/api/more"}, pool);
 * 
 * // Or with async requests
 * auto future1 = cpr::GetAsync(cpr::Url{"http://example.com/api/data"}, pool);
 * auto future2 = cpr::GetAsync(cpr::Url{"http://example.com/api/more"}, pool);
 * ```
 **/
class ConnectionPool {
  public:
    /**
     * Creates a new connection pool with shared connection state.
     * Initializes the underlying CURLSH handle and sets up thread-safe locking mechanisms.
     **/
    ConnectionPool();
    
    /**
     * Copy constructor - creates a new connection pool sharing the same connection state.
     * Multiple ConnectionPool instances can share the same underlying connection pool.
     **/
    ConnectionPool(const ConnectionPool&) = default;
    
    /**
     * Copy assignment operator is deleted to prevent accidental copying.
     * Use the copy constructor if you need to share the connection pool.
     **/
    ConnectionPool& operator=(const ConnectionPool&) = delete;
    
    /**
     * Configures a CURL easy handle to use this connection pool.
     * This method sets up the easy handle to participate in connection sharing
     * managed by this pool.
     * 
     * @param easy_handler The CURL easy handle to configure for connection sharing.
     **/
    void SetupHandler(CURL* easy_handler) const;

  private:
    /**
     * Thread-safe mutex used for synchronizing access to shared connections.
     * This mutex is passed to libcurl's locking callbacks to ensure thread safety
     * when multiple threads access the same connection pool. It's declared first
     * to ensure it's destroyed last, after the CURLSH handle that references it.
     **/
    std::shared_ptr<std::mutex> connection_mutex_;
    
    /**
     * Shared CURL handle (CURLSH) that manages the actual connection sharing.
     * This handle maintains the pool of reusable connections and is configured
     * with appropriate locking callbacks for thread safety. The shared_ptr uses
     * a custom deleter that safely resets the lock/unlock callbacks before
     * calling curl_share_cleanup() to prevent use-after-free issues during destruction.
     * Declared last to ensure it's destroyed first, before the mutex it references.
     **/
    std::shared_ptr<CURLSH> curl_sh_;
};
} // namespace cpr
#endif 