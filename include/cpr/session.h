#ifndef CPR_SESSION_H
#define CPR_SESSION_H

#include <cstdint>
#include <fstream>
#include <memory>

#include "cpr/auth.h"
#include "cpr/bearer.h"
#include "cpr/body.h"
#include "cpr/callback.h"
#include "cpr/connect_timeout.h"
#include "cpr/cookies.h"
#include "cpr/cprtypes.h"
#include "cpr/curlholder.h"
#include "cpr/digest.h"
#include "cpr/limit_rate.h"
#include "cpr/low_speed.h"
#include "cpr/max_redirects.h"
#include "cpr/multipart.h"
#include "cpr/ntlm.h"
#include "cpr/parameters.h"
#include "cpr/payload.h"
#include "cpr/proxies.h"
#include "cpr/proxyauth.h"
#include "cpr/response.h"
#include "cpr/ssl_options.h"
#include "cpr/timeout.h"
#include "cpr/unix_socket.h"
#include "cpr/user_agent.h"
#include "cpr/verbose.h"

namespace cpr {

class Session {
  public:
    Session();
    Session(Session&& old) noexcept;
    Session(const Session& other) = delete;

    ~Session();

    Session& operator=(Session&& old) noexcept;
    Session& operator=(const Session& other) = delete;

    void SetUrl(const Url& url);
    void SetParameters(const Parameters& parameters);
    void SetParameters(Parameters&& parameters);
    void SetHeader(const Header& header);
    void UpdateHeader(const Header& header);
    void SetTimeout(const Timeout& timeout);
    void SetConnectTimeout(const ConnectTimeout& timeout);
    void SetAuth(const Authentication& auth);
    void SetDigest(const Digest& auth);
    void SetUserAgent(const UserAgent& ua);
    void SetPayload(Payload&& payload);
    void SetPayload(const Payload& payload);
    void SetProxies(Proxies&& proxies);
    void SetProxies(const Proxies& proxies);
    void SetProxyAuth(ProxyAuthentication&& proxy_auth);
    void SetProxyAuth(const ProxyAuthentication& proxy_auth);
    void SetMultipart(Multipart&& multipart);
    void SetMultipart(const Multipart& multipart);
    void SetNTLM(const NTLM& auth);
    void SetRedirect(const bool& redirect);
    void SetMaxRedirects(const MaxRedirects& max_redirects);
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
    void SetOption(const Digest& auth);
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
    void SetOption(const NTLM& auth);
    void SetOption(const bool& redirect);
    void SetOption(const MaxRedirects& max_redirects);
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

    Response Delete();
    Response Download(const WriteCallback& write);
    Response Download(std::ofstream& file);
    Response Get();
    Response Head();
    Response Options();
    Response Patch();
    Response Post();
    Response Put();

    std::shared_ptr<CurlHolder> GetCurlHolder();

    void PrepareDelete();
    void PrepareGet();
    void PrepareHead();
    void PrepareOptions();
    void PreparePatch();
    void PreparePost();
    void PreparePut();
    Response Complete(CURLcode curl_error);

  private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace cpr

#endif
