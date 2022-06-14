#ifndef CPR_SESSION_H
#define CPR_SESSION_H

#include <cstdint>
#include <fstream>
#include <future>
#include <memory>
#include <queue>

#include "cpr/accept_encoding.h"
#include "cpr/auth.h"
#include "cpr/bearer.h"
#include "cpr/body.h"
#include "cpr/callback.h"
#include "cpr/connect_timeout.h"
#include "cpr/cookies.h"
#include "cpr/cprtypes.h"
#include "cpr/curlholder.h"
#include "cpr/http_version.h"
#include "cpr/interface.h"
#include "cpr/limit_rate.h"
#include "cpr/local_port.h"
#include "cpr/local_port_range.h"
#include "cpr/low_speed.h"
#include "cpr/multipart.h"
#include "cpr/parameters.h"
#include "cpr/payload.h"
#include "cpr/proxies.h"
#include "cpr/proxyauth.h"
#include "cpr/range.h"
#include "cpr/redirect.h"
#include "cpr/reserve_size.h"
#include "cpr/response.h"
#include "cpr/ssl_options.h"
#include "cpr/timeout.h"
#include "cpr/unix_socket.h"
#include "cpr/user_agent.h"
#include "cpr/verbose.h"

namespace cpr {

using AsyncResponse = std::future<Response>;

class Interceptor;

class Session : public std::enable_shared_from_this<Session> {
  public:
    Session();
    Session(const Session& other) = delete;

    ~Session() = default;

    Session& operator=(Session&& old) noexcept = default;
    Session& operator=(const Session& other) = delete;

    void SetUrl(const Url& url);
    void SetParameters(const Parameters& parameters);
    void SetParameters(Parameters&& parameters);
    void SetHeader(const Header& header);
    void UpdateHeader(const Header& header);
    void SetTimeout(const Timeout& timeout);
    void SetConnectTimeout(const ConnectTimeout& timeout);
    void SetAuth(const Authentication& auth);
// Only supported with libcurl >= 7.61.0.
// As an alternative use SetHeader and add the token manually.
#if LIBCURL_VERSION_NUM >= 0x073D00
    void SetBearer(const Bearer& token);
#endif
    void SetUserAgent(const UserAgent& ua);
    void SetPayload(Payload&& payload);
    void SetPayload(const Payload& payload);
    void SetProxies(Proxies&& proxies);
    void SetProxies(const Proxies& proxies);
    void SetProxyAuth(ProxyAuthentication&& proxy_auth);
    void SetProxyAuth(const ProxyAuthentication& proxy_auth);
    void SetMultipart(Multipart&& multipart);
    void SetMultipart(const Multipart& multipart);
    void SetRedirect(const Redirect& redirect);
    void SetCookies(const Cookies& cookies);
    void SetBody(Body&& body);
    void SetBody(const Body& body);
    void SetLowSpeed(const LowSpeed& low_speed);
    void SetVerifySsl(const VerifySsl& verify);
    void SetUnixSocket(const UnixSocket& unix_socket);
    void SetSslOptions(const SslOptions& options);
    void SetReadCallback(const ReadCallback& read);
    void SetHeaderCallback(const HeaderCallback& header);
    void SetWriteCallback(const WriteCallback& write);
    void SetProgressCallback(const ProgressCallback& progress);
    void SetDebugCallback(const DebugCallback& debug);
    void SetVerbose(const Verbose& verbose);
    void SetInterface(const Interface& iface);
    void SetLocalPort(const LocalPort& local_port);
    void SetLocalPortRange(const LocalPortRange& local_port_range);
    void SetHttpVersion(const HttpVersion& version);
    void SetRange(const Range& range);
    void SetMultiRange(const MultiRange& multi_range);
    void SetReserveSize(const ReserveSize& reserve_size);
    void SetAcceptEncoding(const AcceptEncoding& accept_encoding);
    void SetAcceptEncoding(AcceptEncoding&& accept_encoding);
    void SetLimitRate(const LimitRate& limit_rate);

    // Used in templated functions
    void SetOption(const Url& url);
    void SetOption(const Parameters& parameters);
    void SetOption(Parameters&& parameters);
    void SetOption(const Header& header);
    void SetOption(const Timeout& timeout);
    void SetOption(const ConnectTimeout& timeout);
    void SetOption(const Authentication& auth);
// Only supported with libcurl >= 7.61.0.
// As an alternative use SetHeader and add the token manually.
#if LIBCURL_VERSION_NUM >= 0x073D00
    void SetOption(const Bearer& auth);
#endif
    void SetOption(const UserAgent& ua);
    void SetOption(Payload&& payload);
    void SetOption(const Payload& payload);
    void SetOption(const LimitRate& limit_rate);
    void SetOption(Proxies&& proxies);
    void SetOption(const Proxies& proxies);
    void SetOption(ProxyAuthentication&& proxy_auth);
    void SetOption(const ProxyAuthentication& proxy_auth);
    void SetOption(Multipart&& multipart);
    void SetOption(const Multipart& multipart);
    void SetOption(const Redirect& redirect);
    void SetOption(const Cookies& cookies);
    void SetOption(Body&& body);
    void SetOption(const Body& body);
    void SetOption(const ReadCallback& read);
    void SetOption(const HeaderCallback& header);
    void SetOption(const WriteCallback& write);
    void SetOption(const ProgressCallback& progress);
    void SetOption(const DebugCallback& debug);
    void SetOption(const LowSpeed& low_speed);
    void SetOption(const VerifySsl& verify);
    void SetOption(const Verbose& verbose);
    void SetOption(const UnixSocket& unix_socket);
    void SetOption(const SslOptions& options);
    void SetOption(const Interface& iface);
    void SetOption(const LocalPort& local_port);
    void SetOption(const LocalPortRange& local_port_range);
    void SetOption(const HttpVersion& version);
    void SetOption(const Range& range);
    void SetOption(const MultiRange& multi_range);
    void SetOption(const ReserveSize& reserve_size);
    void SetOption(const AcceptEncoding& accept_encoding);
    void SetOption(AcceptEncoding&& accept_encoding);

    cpr_off_t GetDownloadFileLength();
    /**
     * Attempt to preallocate enough memory for specified number of characters in the response string.
     * Pass 0 to disable this behavior and let the response string be allocated dynamically on demand.
     *
     * Example:
     * cpr::Session session;
     * session.SetUrl(cpr::Url{"http://xxx/file"});
     * session.ResponseStringReserve(1024 * 512); // Reserve space for at least 1024 * 512 characters
     * cpr::Response r = session.Get();
     **/
    void ResponseStringReserve(size_t size);
    Response Delete();
    Response Download(const WriteCallback& write);
    Response Download(std::ofstream& file);
    Response Get();
    Response Head();
    Response Options();
    Response Patch();
    Response Post();
    Response Put();

    AsyncResponse GetAsync();
    AsyncResponse DeleteAsync();
    AsyncResponse DownloadAsync(const WriteCallback& write);
    AsyncResponse DownloadAsync(std::ofstream& file);
    AsyncResponse HeadAsync();
    AsyncResponse OptionsAsync();
    AsyncResponse PatchAsync();
    AsyncResponse PostAsync();
    AsyncResponse PutAsync();

    template <typename Then>
    auto GetCallback(Then then) -> std::future<decltype(then(Get()))>;
    template <typename Then>
    auto PostCallback(Then then) -> std::future<decltype(then(Post()))>;
    template <typename Then>
    auto PutCallback(Then then) -> std::future<decltype(then(Put()))>;
    template <typename Then>
    auto HeadCallback(Then then) -> std::future<decltype(then(Head()))>;
    template <typename Then>
    auto DeleteCallback(Then then) -> std::future<decltype(then(Delete()))>;
    template <typename Then>
    auto OptionsCallback(Then then) -> std::future<decltype(then(Options()))>;
    template <typename Then>
    auto PatchCallback(Then then) -> std::future<decltype(then(Patch()))>;

    std::shared_ptr<CurlHolder> GetCurlHolder();
    std::string GetFullRequestUrl();

    void PrepareDelete();
    void PrepareGet();
    void PrepareHead();
    void PrepareOptions();
    void PreparePatch();
    void PreparePost();
    void PreparePut();
    Response Complete(CURLcode curl_error);

    void AddInterceptor(const std::shared_ptr<Interceptor>& pinterceptor);

  private:
    // Interceptors should be able to call the private procceed() function
    friend Interceptor;

    bool hasBodyOrPayload_{false};
    bool chunkedTransferEncoding_{false};
    std::shared_ptr<CurlHolder> curl_;
    Url url_;
    Parameters parameters_;
    Proxies proxies_;
    ProxyAuthentication proxyAuth_;
    Header header_;
    AcceptEncoding acceptEncoding_;
    /**
     * Will be set by the read callback.
     * Ensures that the "Transfer-Encoding" is set to "chunked", if not overriden in header_.
     **/
    ReadCallback readcb_;
    HeaderCallback headercb_;
    WriteCallback writecb_;
    ProgressCallback progresscb_;
    DebugCallback debugcb_;
    size_t response_string_reserve_size_{0};
    std::string response_string_;
    std::string header_string_;
    std::queue<std::shared_ptr<Interceptor>> interceptors_;

    Response makeDownloadRequest();
    Response makeRequest();
    Response proceed();
    void prepareCommon();
    void SetHeaderInternal();
    std::shared_ptr<Session> GetSharedPtrFromThis();
};

template <typename Then>
auto Session::GetCallback(Then then) -> std::future<decltype(then(Get()))> {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this](Then then_inner) { return then_inner(shared_this->Get()); }, std::move(then));
}

template <typename Then>
auto Session::PostCallback(Then then) -> std::future<decltype(then(Post()))> {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this](Then then_inner) { return then_inner(shared_this->Post()); }, std::move(then));
}

template <typename Then>
auto Session::PutCallback(Then then) -> std::future<decltype(then(Put()))> {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this](Then then_inner) { return then_inner(shared_this->Put()); }, std::move(then));
}

template <typename Then>
auto Session::HeadCallback(Then then) -> std::future<decltype(then(Head()))> {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this](Then then_inner) { return then_inner(shared_this->Head()); }, std::move(then));
}

template <typename Then>
auto Session::DeleteCallback(Then then) -> std::future<decltype(then(Delete()))> {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this](Then then_inner) { return then_inner(shared_this->Delete()); }, std::move(then));
}

template <typename Then>
auto Session::OptionsCallback(Then then) -> std::future<decltype(then(Options()))> {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this](Then then_inner) { return then_inner(shared_this->Options()); }, std::move(then));
}

template <typename Then>
auto Session::PatchCallback(Then then) -> std::future<decltype(then(Patch()))> {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this](Then then_inner) { return then_inner(shared_this->Patch()); }, std::move(then));
}

} // namespace cpr

#endif
