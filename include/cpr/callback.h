#ifndef CPR_CALLBACK_H
#define CPR_CALLBACK_H

#include "cpr/cprtypes.h"
#include "cpr/curlholder.h"

#include <atomic>
#include <cassert>
#include <functional>
#include <optional>
#include <utility>

/**
 * Needs to be defined here instead of inside ssl_options.h to avoid having to include ssl_options.h leading to a circular include.
 **/
#ifdef CPR_SSL_CTX_CALLBACK_ENABLED
#ifndef SUPPORT_CURLOPT_SSL_CTX_FUNCTION
#define SUPPORT_CURLOPT_SSL_CTX_FUNCTION LIBCURL_VERSION_NUM >= 0x070B00 // 7.11.0
#else
#define SUPPORT_CURLOPT_SSL_CTX_FUNCTION false
#endif
#endif

namespace cpr {
class ReadCallback {
  public:
    ReadCallback() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    ReadCallback(std::function<bool(char* buffer, size_t& size, intptr_t userdata)> p_callback, intptr_t p_userdata = 0) : userdata(p_userdata), size{-1}, callback{std::move(p_callback)} {}
    ReadCallback(cpr_off_t p_size, std::function<bool(char* buffer, size_t& size, intptr_t userdata)> p_callback, intptr_t p_userdata = 0) : userdata(p_userdata), size{p_size}, callback{std::move(p_callback)} {}
    bool operator()(char* buffer, size_t& buffer_size) const {
        return callback(buffer, buffer_size, userdata);
    }

    intptr_t userdata{};
    cpr_off_t size{};
    std::function<bool(char* buffer, size_t& size, intptr_t userdata)> callback;
};

class HeaderCallback {
  public:
    HeaderCallback() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    HeaderCallback(std::function<bool(std::string header, intptr_t userdata)> p_callback, intptr_t p_userdata = 0) : userdata(p_userdata), callback(std::move(p_callback)) {}
    bool operator()(std::string header) const {
        return callback(std::move(header), userdata);
    }

    intptr_t userdata{};
    std::function<bool(std::string header, intptr_t userdata)> callback;
};

class WriteCallback {
  public:
    WriteCallback() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    WriteCallback(std::function<bool(std::string data, intptr_t userdata)> p_callback, intptr_t p_userdata = 0) : userdata(p_userdata), callback(std::move(p_callback)) {}
    bool operator()(std::string data) const {
        return callback(std::move(data), userdata);
    }

    intptr_t userdata{};
    std::function<bool(std::string data, intptr_t userdata)> callback;
};

class ProgressCallback {
  public:
    ProgressCallback() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    ProgressCallback(std::function<bool(cpr_pf_arg_t downloadTotal, cpr_pf_arg_t downloadNow, cpr_pf_arg_t uploadTotal, cpr_pf_arg_t uploadNow, intptr_t userdata)> p_callback, intptr_t p_userdata = 0) : userdata(p_userdata), callback(std::move(p_callback)) {}
    bool operator()(cpr_pf_arg_t downloadTotal, cpr_pf_arg_t downloadNow, cpr_pf_arg_t uploadTotal, cpr_pf_arg_t uploadNow) const {
        return callback(downloadTotal, downloadNow, uploadTotal, uploadNow, userdata);
    }

    intptr_t userdata{};
    std::function<bool(cpr_pf_arg_t downloadTotal, cpr_pf_arg_t downloadNow, cpr_pf_arg_t uploadTotal, cpr_pf_arg_t uploadNow, intptr_t userdata)> callback;
};

class DebugCallback {
  public:
    enum class InfoType {
        TEXT = 0,
        HEADER_IN = 1,
        HEADER_OUT = 2,
        DATA_IN = 3,
        DATA_OUT = 4,
        SSL_DATA_IN = 5,
        SSL_DATA_OUT = 6,
    };
    DebugCallback() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    DebugCallback(std::function<void(InfoType type, std::string data, intptr_t userdata)> p_callback, intptr_t p_userdata = 0) : userdata(p_userdata), callback(std::move(p_callback)) {}
    void operator()(InfoType type, std::string data) const {
        callback(type, std::move(data), userdata);
    }

    intptr_t userdata{};
    std::function<void(InfoType type, std::string data, intptr_t userdata)> callback;
};

/**
 * Functor class for progress functions that will be used in cancellable requests.
 */
class CancellationCallback {
  public:
    CancellationCallback() = default;
    explicit CancellationCallback(std::shared_ptr<std::atomic_bool>&& cs) : cancellation_state{std::move(cs)} {}

    CancellationCallback(std::shared_ptr<std::atomic_bool>&& cs, ProgressCallback& u_cb) : cancellation_state{std::move(cs)}, user_cb{std::reference_wrapper{u_cb}} {}

    bool operator()(cpr_pf_arg_t dltotal, cpr_pf_arg_t dlnow, cpr_pf_arg_t ultotal, cpr_pf_arg_t ulnow) const;

    void SetProgressCallback(ProgressCallback& u_cb);

  private:
    std::shared_ptr<std::atomic_bool> cancellation_state;
    std::optional<std::reference_wrapper<ProgressCallback>> user_cb;
};

#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION
namespace ssl {
/**
 * This callback function gets called by libcurl just before the initialization of an SSL connection
 * after having processed all other SSL related options to give a last chance to an application
 * to modify the behavior of the SSL initialization.
 *
 * If an error is returned from the callback no attempt to establish a connection is made
 * and the perform operation returns the callback's error code.
 * For no error return CURLE_OK from inside 'curl/curl.h'
 *
 * More/Source: https://curl.se/libcurl/c/CURLOPT_SSL_CTX_FUNCTION.html
 **/
class SslCtxCallback {
  public:
    std::function<CURLcode(const std::shared_ptr<CurlHolder>& curl_holder, void* ssl_ctx, intptr_t userdata)> callback{};
    intptr_t userdata{};
    std::shared_ptr<CurlHolder> curl_holder{nullptr};

    SslCtxCallback() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    SslCtxCallback(const std::function<CURLcode(const std::shared_ptr<CurlHolder>& p_curl_holder, void* p_ssl_ctx, intptr_t p_userdata)>& p_callback, intptr_t p_userdata = 0) : callback(p_callback), userdata(p_userdata) {}

    CURLcode operator()(CURL* p_curl, void* p_ssl_ctx) const {
        // We use our own way of passing arguments curl and the client pointer to the function.
        assert(p_curl == curl_holder->handle);
        (void) p_curl;

        return callback(curl_holder, p_ssl_ctx, userdata);
    }

    void SetCurlHolder(const std::shared_ptr<CurlHolder>& p_curl_holder) {
        this->curl_holder = p_curl_holder;
    }
};

#ifdef OPENSSL_BACKEND_USED
CURLcode tryLoadCaCertFromBuffer(CURL* curl, void* sslctx, void* raw_cert_buf);
#endif
} // namespace ssl
#endif
} // namespace cpr

#endif
