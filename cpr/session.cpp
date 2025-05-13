#include "cpr/session.h"

#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <curl/curl.h>
#include <curl/curlver.h>
#include <curl/easy.h>
#include <curl/system.h>

#include "cpr/accept_encoding.h"
#include "cpr/async.h"
#include "cpr/auth.h"
#include "cpr/bearer.h"
#include "cpr/body.h"
#include "cpr/body_view.h"
#include "cpr/callback.h"
#include "cpr/connect_timeout.h"
#include "cpr/cookies.h"
#include "cpr/cprtypes.h"
#include "cpr/curlholder.h"
#include "cpr/error.h"
#include "cpr/file.h"
#include "cpr/filesystem.h" // IWYU pragma: keep
#include "cpr/http_version.h"
#include "cpr/interceptor.h"
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
#include "cpr/resolve.h"
#include "cpr/response.h"
#include "cpr/ssl_options.h"
#include "cpr/timeout.h"
#include "cpr/unix_socket.h"
#include "cpr/user_agent.h"
#include "cpr/util.h"
#include "cpr/verbose.h"

#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION
#include "cpr/ssl_ctx.h"
#endif


namespace cpr {
// Ignored here since libcurl reqires a long:
// NOLINTNEXTLINE(google-runtime-int)
constexpr long ON = 1L;
// Ignored here since libcurl reqires a long:
// NOLINTNEXTLINE(google-runtime-int)
constexpr long OFF = 0L;

CURLcode Session::DoEasyPerform() {
    if (isUsedInMultiPerform) {
        std::cerr << "curl_easy_perform cannot be executed if the CURL handle is used in a MultiPerform.\n";
        return CURLcode::CURLE_FAILED_INIT;
    }
    return curl_easy_perform(curl_->handle);
}

void Session::prepareHeader() {
    curl_slist* chunk = nullptr;
    for (const std::pair<const std::string, std::string>& item : header_) {
        std::string header_string = item.first;
        if (item.second.empty()) {
            header_string += ";";
        } else {
            header_string += ": " + item.second;
        }

        curl_slist* temp = curl_slist_append(chunk, header_string.c_str());
        if (temp) {
            chunk = temp;
        }
    }

    // Set the chunked transfer encoding in case it does not already exist:
    if (chunkedTransferEncoding_ && header_.find("Transfer-Encoding") == header_.end()) {
        curl_slist* temp = curl_slist_append(chunk, "Transfer-Encoding:chunked");
        if (temp) {
            chunk = temp;
        }
    }

    // libcurl would prepare the header "Expect: 100-continue" by default when uploading files larger than 1 MB.
    // Here we would like to disable this feature:
    curl_slist* temp = curl_slist_append(chunk, "Expect:");
    if (temp) {
        chunk = temp;
    }

    curl_easy_setopt(curl_->handle, CURLOPT_HTTPHEADER, chunk);

    curl_slist_free_all(curl_->chunk);
    curl_->chunk = chunk;
}

void Session::prepareProxy() {
    const std::string protocol = url_.str().substr(0, url_.str().find(':'));
    if (proxies_.has(protocol)) {
        curl_easy_setopt(curl_->handle, CURLOPT_PROXY, proxies_[protocol].c_str());
        if (proxyAuth_.has(protocol)) {
            curl_easy_setopt(curl_->handle, CURLOPT_PROXYUSERNAME, proxyAuth_.GetUsernameUnderlying(protocol).c_str());
            curl_easy_setopt(curl_->handle, CURLOPT_PROXYPASSWORD, proxyAuth_.GetPasswordUnderlying(protocol).c_str());
        }
    }
}

// Only supported with libcurl >= 7.61.0.
// As an alternative use SetHeader and add the token manually.
#if LIBCURL_VERSION_NUM >= 0x073D00
void Session::SetBearer(const Bearer& token) {
    // Ignore here since this has been defined by libcurl.
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPAUTH, CURLAUTH_BEARER);
    curl_easy_setopt(curl_->handle, CURLOPT_XOAUTH2_BEARER, token.GetToken());
}
#endif

Session::Session() : curl_(new CurlHolder()) {
    // Set up some sensible defaults
    curl_version_info_data* version_info = curl_version_info(CURLVERSION_NOW);
    const std::string version = "curl/" + std::string{version_info->version};
    curl_easy_setopt(curl_->handle, CURLOPT_USERAGENT, version.c_str());
    SetRedirect(Redirect());
    curl_easy_setopt(curl_->handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_->handle, CURLOPT_ERRORBUFFER, curl_->error.data());
    curl_easy_setopt(curl_->handle, CURLOPT_COOKIEFILE, "");
#ifdef CPR_CURL_NOSIGNAL
    curl_easy_setopt(curl_->handle, CURLOPT_NOSIGNAL, 1L);
#endif

#if LIBCURL_VERSION_NUM >= 0x071900 // 7.25.0
    curl_easy_setopt(curl_->handle, CURLOPT_TCP_KEEPALIVE, 1L);
#endif
    current_interceptor_ = interceptors_.end();
    first_interceptor_ = interceptors_.end();
}

Response Session::makeDownloadRequest() {
    const std::optional<Response> r = intercept();
    if (r.has_value()) {
        return r.value();
    }

    const CURLcode curl_error = DoEasyPerform();

    return CompleteDownload(curl_error);
}

void Session::prepareCommonShared() {
    assert(curl_->handle);

    // Set Header:
    prepareHeader();

    // URL parameter:
    const std::string parametersContent = parameters_.GetContent(*curl_);
    if (!parametersContent.empty()) {
        const Url new_url{url_ + "?" + parametersContent};
        curl_easy_setopt(curl_->handle, CURLOPT_URL, new_url.c_str());
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_URL, url_.c_str());
    }

    // Proxy:
    prepareProxy();

    // handle NO_PROXY override passed through Proxies object
    // Example: Proxies{"no_proxy": ""} will override environment variable definition with an empty list
    const std::array<std::string, 2> no_proxy{"no_proxy", "NO_PROXY"};
    for (const auto& item : no_proxy) { // cppcheck-suppress useStlAlgorithm
        if (proxies_.has(item)) {       // cppcheck-suppress useStlAlgorithm
            curl_easy_setopt(curl_->handle, CURLOPT_NOPROXY, proxies_[item].c_str());
            break;
        }
    }

#if LIBCURL_VERSION_NUM >= 0x071506 // 7.21.6
    if (acceptEncoding_.empty()) {
        // Enable all supported built-in compressions
        curl_easy_setopt(curl_->handle, CURLOPT_ACCEPT_ENCODING, "");
    } else if (acceptEncoding_.disabled()) {
        // Disable curl adding the 'Accept-Encoding' header
        curl_easy_setopt(curl_->handle, CURLOPT_ACCEPT_ENCODING, nullptr);
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_ACCEPT_ENCODING, acceptEncoding_.getString().c_str());
    }
#endif

    curl_->error[0] = '\0';

    // Clear the response
    response_string_.clear();
    if (response_string_reserve_size_ > 0) {
        response_string_.reserve(response_string_reserve_size_);
    }

    // Enable so we are able to retrieve certificate information:
    curl_easy_setopt(curl_->handle, CURLOPT_CERTINFO, 1L);
}

void Session::prepareCommon() {
    assert(curl_->handle);

    // Everything else:
    prepareCommonShared();

    // Set Content:
    prepareBodyPayloadOrMultipart();

    if (!cbs_->writecb_.callback) {
        curl_easy_setopt(curl_->handle, CURLOPT_WRITEFUNCTION, cpr::util::writeFunction);
        curl_easy_setopt(curl_->handle, CURLOPT_WRITEDATA, &response_string_);
    }

    header_string_.clear();
    if (!cbs_->headercb_.callback) {
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERFUNCTION, cpr::util::writeFunction);
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERDATA, &header_string_);
    }
}

void Session::prepareCommonDownload() {
    assert(curl_->handle);

    // Everything else:
    prepareCommonShared();

    header_string_.clear();
    if (cbs_->headercb_.callback) {
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERFUNCTION, cpr::util::headerUserFunction);
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERDATA, &cbs_->headercb_);
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERFUNCTION, cpr::util::writeFunction);
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERDATA, &header_string_);
    }
}

Response Session::makeRequest() {
    const std::optional<Response> r = intercept();
    if (r.has_value()) {
        return r.value();
    }

    const CURLcode curl_error = DoEasyPerform();

    return Complete(curl_error);
}

void Session::SetLimitRate(const LimitRate& limit_rate) {
    curl_easy_setopt(curl_->handle, CURLOPT_MAX_RECV_SPEED_LARGE, limit_rate.downrate);
    curl_easy_setopt(curl_->handle, CURLOPT_MAX_SEND_SPEED_LARGE, limit_rate.uprate);
}

const Content& Session::GetContent() const {
    return content_;
}

void Session::RemoveContent() {
    // inverse function to prepareBodyPayloadOrMultipart()
    if (std::holds_alternative<cpr::Payload>(content_) || std::holds_alternative<cpr::Body>(content_)) {
        // set default values, so curl does not send a body in subsequent requests
        curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE, -1);
        curl_easy_setopt(curl_->handle, CURLOPT_COPYPOSTFIELDS, nullptr);
    } else if (std::holds_alternative<cpr::Multipart>(content_)) {
        if (curl_->multipart) {
            // remove multipart data
            curl_mime_free(curl_->multipart);
            curl_->multipart = nullptr;
        }
    } else if (std::holds_alternative<cpr::BodyView>(content_)) {
        // set default values, so curl does not send a body in subsequent requests
        curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE, -1);
        curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDS, nullptr);
    }
    content_ = std::monostate{};
}

void Session::SetReadCallback(const ReadCallback& read) {
    cbs_->readcb_ = read;
    curl_easy_setopt(curl_->handle, CURLOPT_INFILESIZE_LARGE, read.size);
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE, read.size);
    curl_easy_setopt(curl_->handle, CURLOPT_READFUNCTION, cpr::util::readUserFunction);
    curl_easy_setopt(curl_->handle, CURLOPT_READDATA, &cbs_->readcb_);
    chunkedTransferEncoding_ = read.size == -1;
}

void Session::SetHeaderCallback(const HeaderCallback& header) {
    curl_easy_setopt(curl_->handle, CURLOPT_HEADERFUNCTION, cpr::util::headerUserFunction);
    cbs_->headercb_ = header;
    curl_easy_setopt(curl_->handle, CURLOPT_HEADERDATA, &cbs_->headercb_);
}

void Session::SetWriteCallback(const WriteCallback& write) {
    curl_easy_setopt(curl_->handle, CURLOPT_WRITEFUNCTION, cpr::util::writeUserFunction);
    cbs_->writecb_ = write;
    curl_easy_setopt(curl_->handle, CURLOPT_WRITEDATA, &cbs_->writecb_);
}

void Session::SetProgressCallback(const ProgressCallback& progress) {
    cbs_->progresscb_ = progress;
    if (isCancellable) {
        cbs_->cancellationcb_.SetProgressCallback(cbs_->progresscb_);
        return;
    }
#if LIBCURL_VERSION_NUM < 0x072000 // 7.32.0
    curl_easy_setopt(curl_->handle, CURLOPT_PROGRESSFUNCTION, cpr::util::progressUserFunction<ProgressCallback>);
    curl_easy_setopt(curl_->handle, CURLOPT_PROGRESSDATA, &cbs_->progresscb_);
#else
    curl_easy_setopt(curl_->handle, CURLOPT_XFERINFOFUNCTION, cpr::util::progressUserFunction<ProgressCallback>);
    curl_easy_setopt(curl_->handle, CURLOPT_XFERINFODATA, &cbs_->progresscb_);
#endif
    curl_easy_setopt(curl_->handle, CURLOPT_NOPROGRESS, 0L);
}

void Session::SetDebugCallback(const DebugCallback& debug) {
    curl_easy_setopt(curl_->handle, CURLOPT_DEBUGFUNCTION, cpr::util::debugUserFunction);
    cbs_->debugcb_ = debug;
    curl_easy_setopt(curl_->handle, CURLOPT_DEBUGDATA, &cbs_->debugcb_);
    curl_easy_setopt(curl_->handle, CURLOPT_VERBOSE, 1L);
}

void Session::SetUrl(const Url& url) {
    url_ = url;
}

void Session::SetResolve(const Resolve& resolve) {
    SetResolves({resolve});
}

void Session::SetResolves(const std::vector<Resolve>& resolves) {
    curl_slist_free_all(curl_->resolveCurlList);
    curl_->resolveCurlList = nullptr;
    for (const Resolve& resolve : resolves) {
        for (const uint16_t port : resolve.ports) {
            curl_->resolveCurlList = curl_slist_append(curl_->resolveCurlList, (resolve.host + ":" + std::to_string(port) + ":" + resolve.addr).c_str());
        }
    }
    curl_easy_setopt(curl_->handle, CURLOPT_RESOLVE, curl_->resolveCurlList);
}

void Session::SetParameters(const Parameters& parameters) {
    parameters_ = parameters;
}

void Session::SetParameters(Parameters&& parameters) {
    parameters_ = std::move(parameters);
}

void Session::SetHeader(const Header& header) {
    header_ = header;
}

void Session::UpdateHeader(const Header& header) {
    for (const std::pair<const std::string, std::string>& item : header) {
        header_[item.first] = item.second;
    }
}

Header& Session::GetHeader() {
    return header_;
}

const Header& Session::GetHeader() const {
    return header_;
}

void Session::SetTimeout(const Timeout& timeout) {
    curl_easy_setopt(curl_->handle, CURLOPT_TIMEOUT_MS, timeout.Milliseconds());
}

void Session::SetConnectTimeout(const ConnectTimeout& timeout) {
    curl_easy_setopt(curl_->handle, CURLOPT_CONNECTTIMEOUT_MS, timeout.Milliseconds());
}

void Session::SetAuth(const Authentication& auth) {
    // Ignore here since this has been defined by libcurl.
    switch (auth.GetAuthMode()) {
        case AuthMode::BASIC:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_easy_setopt(curl_->handle, CURLOPT_USERPWD, auth.GetAuthString());
            break;
        case AuthMode::DIGEST:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
            curl_easy_setopt(curl_->handle, CURLOPT_USERPWD, auth.GetAuthString());
            break;
        case AuthMode::NTLM:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTPAUTH, CURLAUTH_NTLM);
            curl_easy_setopt(curl_->handle, CURLOPT_USERPWD, auth.GetAuthString());
            break;
        case AuthMode::NEGOTIATE:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTPAUTH, CURLAUTH_NEGOTIATE);
            curl_easy_setopt(curl_->handle, CURLOPT_USERPWD, auth.GetAuthString());
            break;
        case AuthMode::ANY:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
            curl_easy_setopt(curl_->handle, CURLOPT_USERPWD, auth.GetAuthString());
            break;
        case AuthMode::ANYSAFE:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTPAUTH, CURLAUTH_ANYSAFE);
            curl_easy_setopt(curl_->handle, CURLOPT_USERPWD, auth.GetAuthString());
            break;
    }
}

void Session::SetUserAgent(const UserAgent& ua) {
    curl_easy_setopt(curl_->handle, CURLOPT_USERAGENT, ua.c_str());
}

void Session::SetPayload(const Payload& payload) {
    content_ = payload;
}

void Session::SetPayload(Payload&& payload) {
    content_ = std::move(payload);
}

void Session::SetProxies(const Proxies& proxies) {
    proxies_ = proxies;
}

void Session::SetProxies(Proxies&& proxies) {
    proxies_ = std::move(proxies);
}

void Session::SetProxyAuth(ProxyAuthentication&& proxy_auth) {
    proxyAuth_ = std::move(proxy_auth);
}

void Session::SetProxyAuth(const ProxyAuthentication& proxy_auth) {
    proxyAuth_ = proxy_auth;
}

void Session::SetMultipart(const Multipart& multipart) {
    content_ = multipart;
}

void Session::SetMultipart(Multipart&& multipart) {
    content_ = std::move(multipart);
}

void Session::SetRedirect(const Redirect& redirect) {
    curl_easy_setopt(curl_->handle, CURLOPT_FOLLOWLOCATION, redirect.follow ? 1L : 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_MAXREDIRS, redirect.maximum);
    curl_easy_setopt(curl_->handle, CURLOPT_UNRESTRICTED_AUTH, redirect.cont_send_cred ? 1L : 0L);

    // NOLINTNEXTLINE (google-runtime-int)
    long mask = 0;
    if (any(redirect.post_flags & PostRedirectFlags::POST_301)) {
        mask |= CURL_REDIR_POST_301;
    }
    if (any(redirect.post_flags & PostRedirectFlags::POST_302)) {
        mask |= CURL_REDIR_POST_302;
    }
    if (any(redirect.post_flags & PostRedirectFlags::POST_303)) {
        mask |= CURL_REDIR_POST_303;
    }
    curl_easy_setopt(curl_->handle, CURLOPT_POSTREDIR, mask);
}

void Session::SetCookies(const Cookies& cookies) {
    curl_easy_setopt(curl_->handle, CURLOPT_COOKIELIST, "ALL");
    curl_easy_setopt(curl_->handle, CURLOPT_COOKIE, cookies.GetEncoded(*curl_).c_str());
}

void Session::SetBody(const Body& body) {
    content_ = body;
}

void Session::SetBody(Body&& body) {
    content_ = std::move(body);
}

// cppcheck-suppress passedByValue
void Session::SetBodyView(BodyView body) {
    static_assert(std::is_trivially_copyable_v<BodyView>, "BodyView expected to be trivially copyable otherwise will need some std::move across codebase");
    content_ = body;
}

void Session::SetLowSpeed(const LowSpeed& low_speed) {
    curl_easy_setopt(curl_->handle, CURLOPT_LOW_SPEED_LIMIT, static_cast<long>(low_speed.limit));
    curl_easy_setopt(curl_->handle, CURLOPT_LOW_SPEED_TIME, static_cast<long>(low_speed.time.count())); // cppcheck-suppress y2038-unsafe-call
}

void Session::SetVerifySsl(const VerifySsl& verify) {
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_VERIFYPEER, verify ? ON : OFF);
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_VERIFYHOST, verify ? 2L : 0L);
}

void Session::SetUnixSocket(const UnixSocket& unix_socket) {
    curl_easy_setopt(curl_->handle, CURLOPT_UNIX_SOCKET_PATH, unix_socket.GetUnixSocketString());
}

void Session::SetSslOptions(const SslOptions& options) {
    if (!options.cert_file.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_SSLCERT, options.cert_file.c_str());
        if (!options.cert_type.empty()) {
            curl_easy_setopt(curl_->handle, CURLOPT_SSLCERTTYPE, options.cert_type.c_str());
        }
    }
    if (!options.key_file.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_SSLKEY, options.key_file.c_str());
        if (!options.key_type.empty()) {
            curl_easy_setopt(curl_->handle, CURLOPT_SSLKEYTYPE, options.key_type.c_str());
        }
        if (!options.key_pass.empty()) {
            curl_easy_setopt(curl_->handle, CURLOPT_KEYPASSWD, options.key_pass.c_str());
        }
#if SUPPORT_CURLOPT_SSLKEY_BLOB
    } else if (!options.key_blob.empty()) {
        std::string key_blob(options.key_blob);
        curl_blob blob{};
        // NOLINTNEXTLINE (readability-container-data-pointer)
        blob.data = &key_blob[0];
        blob.len = key_blob.length();
        blob.flags = CURL_BLOB_COPY;
        curl_easy_setopt(curl_->handle, CURLOPT_SSLKEY_BLOB, &blob);
        if (!options.key_type.empty()) {
            curl_easy_setopt(curl_->handle, CURLOPT_SSLKEYTYPE, options.key_type.c_str());
        }
        if (!options.key_pass.empty()) {
            curl_easy_setopt(curl_->handle, CURLOPT_KEYPASSWD, options.key_pass.c_str());
        }
#endif
    }
    if (!options.pinned_public_key.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_PINNEDPUBLICKEY, options.pinned_public_key.c_str());
    }
#if SUPPORT_ALPN
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_ENABLE_ALPN, options.enable_alpn ? ON : OFF);
#endif
#if SUPPORT_NPN
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_ENABLE_NPN, options.enable_npn ? ON : OFF);
#endif
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_VERIFYPEER, options.verify_peer ? ON : OFF);
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_VERIFYHOST, options.verify_host ? 2L : 0L);
#if LIBCURL_VERSION_NUM >= 0x072900 // 7.41.0
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_VERIFYSTATUS, options.verify_status ? ON : OFF);
#endif

    int maxTlsVersion = options.ssl_version;
#if SUPPORT_MAX_TLS_VERSION
    maxTlsVersion |= options.max_version;
#endif

    curl_easy_setopt(curl_->handle, CURLOPT_SSLVERSION,
                     // Ignore here since this has been defined by libcurl.
                     maxTlsVersion);

    // NOLINTNEXTLINE (google-runtime-int)
    long curlSslOptions = 0;
#if SUPPORT_SSL_NO_REVOKE
    sslNoRevoke_ = options.ssl_no_revoke;
    if (options.ssl_no_revoke) {
        curlSslOptions |= CURLSSLOPT_NO_REVOKE;
    }
#endif
#if LIBCURL_VERSION_NUM >= 0x074700 // 7.71.0
    // Fix loading certs from Windows cert store when using OpenSSL:
    curlSslOptions |= CURLSSLOPT_NATIVE_CA;
#endif
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_OPTIONS, curlSslOptions);

    if (!options.ca_info.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_CAINFO, options.ca_info.c_str());
    }
    if (!options.ca_path.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_CAPATH, options.ca_path.c_str());
    }
#if SUPPORT_CURLOPT_SSL_CTX_FUNCTION
#ifdef OPENSSL_BACKEND_USED
    if (!options.ca_buffer.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_SSL_CTX_FUNCTION, sslctx_function_load_ca_cert_from_buffer);
        curl_easy_setopt(curl_->handle, CURLOPT_SSL_CTX_DATA, options.ca_buffer.c_str());
    }
#endif
#endif
    if (!options.crl_file.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_CRLFILE, options.crl_file.c_str());
    }
    if (!options.ciphers.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_SSL_CIPHER_LIST, options.ciphers.c_str());
    }
#if SUPPORT_TLSv13_CIPHERS
    if (!options.tls13_ciphers.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_TLS13_CIPHERS, options.tls13_ciphers.c_str());
    }
#endif
#if SUPPORT_SESSIONID_CACHE
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_SESSIONID_CACHE, options.session_id_cache ? ON : OFF);
#endif
}

void Session::SetVerbose(const Verbose& verbose) {
    curl_easy_setopt(curl_->handle, CURLOPT_VERBOSE, verbose.verbose ? ON : OFF);
}

void Session::SetInterface(const Interface& iface) {
    if (iface.str().empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_INTERFACE, nullptr);
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_INTERFACE, iface.c_str());
    }
}

void Session::SetLocalPort(const LocalPort& local_port) {
    curl_easy_setopt(curl_->handle, CURLOPT_LOCALPORT, static_cast<long>(static_cast<uint16_t>(local_port)));
}

void Session::SetLocalPortRange(const LocalPortRange& local_port_range) {
    curl_easy_setopt(curl_->handle, CURLOPT_LOCALPORTRANGE, static_cast<long>(static_cast<uint16_t>(local_port_range)));
}

void Session::SetHttpVersion(const HttpVersion& version) {
    switch (version.code) {
        case HttpVersionCode::VERSION_NONE:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_NONE);
            break;

        case HttpVersionCode::VERSION_1_0:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
            break;

        case HttpVersionCode::VERSION_1_1:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
            break;

#if LIBCURL_VERSION_NUM >= 0x072100 // 7.33.0
        case HttpVersionCode::VERSION_2_0:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
            break;
#endif

#if LIBCURL_VERSION_NUM >= 0x072F00 // 7.47.0
        case HttpVersionCode::VERSION_2_0_TLS:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
            break;
#endif

#if LIBCURL_VERSION_NUM >= 0x073100 // 7.49.0
        case HttpVersionCode::VERSION_2_0_PRIOR_KNOWLEDGE:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE);
            break;
#endif

#if LIBCURL_VERSION_NUM >= 0x074200 // 7.66.0
        case HttpVersionCode::VERSION_3_0:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_3);
            break;
#endif
#if LIBCURL_VERSION_NUM >= 0x075701 // 7.87.1, but corresponds to 7.88.0 tag
        case HttpVersionCode::VERSION_3_0_ONLY:
            curl_easy_setopt(curl_->handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_3ONLY);
            break;
#endif

        default: // Should not happen
            throw std::invalid_argument("Invalid/Unknown HTTP version type.");
    }
}

void Session::SetRange(const Range& range) {
    const std::string range_str = range.str();
    curl_easy_setopt(curl_->handle, CURLOPT_RANGE, range_str.c_str());
}

void Session::SetMultiRange(const MultiRange& multi_range) {
    const std::string multi_range_str = multi_range.str();
    curl_easy_setopt(curl_->handle, CURLOPT_RANGE, multi_range_str.c_str());
}

void Session::SetReserveSize(const ReserveSize& reserve_size) {
    ResponseStringReserve(reserve_size.size);
}

void Session::SetAcceptEncoding(const AcceptEncoding& accept_encoding) {
    acceptEncoding_ = accept_encoding;
}

void Session::SetAcceptEncoding(AcceptEncoding&& accept_encoding) {
    acceptEncoding_ = std::move(accept_encoding);
}

cpr_off_t Session::GetDownloadFileLength() {
    cpr_off_t downloadFileLength = -1;
    curl_easy_setopt(curl_->handle, CURLOPT_URL, url_.c_str());

    prepareProxy();

    curl_easy_setopt(curl_->handle, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 1);
    if (DoEasyPerform() == CURLE_OK) {
        // NOLINTNEXTLINE (google-runtime-int)
        long status_code{};
        curl_easy_getinfo(curl_->handle, CURLINFO_RESPONSE_CODE, &status_code);
        if (200 == status_code) {
            curl_easy_getinfo(curl_->handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &downloadFileLength);
        }
    }
    return downloadFileLength;
}

void Session::ResponseStringReserve(size_t size) {
    response_string_reserve_size_ = size;
}

Response Session::Delete() {
    PrepareDelete();
    return makeRequest();
}

Response Session::Download(const WriteCallback& write) {
    PrepareDownload(write);
    return makeDownloadRequest();
}

Response Session::Download(std::ofstream& file) {
    PrepareDownload(file);
    return makeDownloadRequest();
}

Response Session::Get() {
    PrepareGet();
    return makeRequest();
}

Response Session::Head() {
    PrepareHead();
    return makeRequest();
}

Response Session::Options() {
    PrepareOptions();
    return makeRequest();
}

Response Session::Patch() {
    PreparePatch();
    return makeRequest();
}

Response Session::Post() {
    PreparePost();
    return makeRequest();
}

Response Session::Put() {
    PreparePut();
    return makeRequest();
}

std::shared_ptr<Session> Session::GetSharedPtrFromThis() {
    try {
        return shared_from_this();
    } catch (std::bad_weak_ptr&) {
        throw std::runtime_error("Failed to get a shared pointer from this. The reason is probably that the session object is not managed by a shared pointer, which is required to use this functionality.");
    }
}

AsyncResponse Session::GetAsync() {
    auto shared_this = shared_from_this();
    return async([shared_this]() { return shared_this->Get(); });
}

AsyncResponse Session::DeleteAsync() {
    return async([shared_this = GetSharedPtrFromThis()]() { return shared_this->Delete(); });
}

AsyncResponse Session::DownloadAsync(const WriteCallback& write) {
    return async([shared_this = GetSharedPtrFromThis(), write]() { return shared_this->Download(write); });
}

AsyncResponse Session::DownloadAsync(std::ofstream& file) {
    return async([shared_this = GetSharedPtrFromThis(), &file]() { return shared_this->Download(file); });
}

AsyncResponse Session::HeadAsync() {
    return async([shared_this = GetSharedPtrFromThis()]() { return shared_this->Head(); });
}

AsyncResponse Session::OptionsAsync() {
    return async([shared_this = GetSharedPtrFromThis()]() { return shared_this->Options(); });
}

AsyncResponse Session::PatchAsync() {
    return async([shared_this = GetSharedPtrFromThis()]() { return shared_this->Patch(); });
}

AsyncResponse Session::PostAsync() {
    return async([shared_this = GetSharedPtrFromThis()]() { return shared_this->Post(); });
}

AsyncResponse Session::PutAsync() {
    return async([shared_this = GetSharedPtrFromThis()]() { return shared_this->Put(); });
}

std::shared_ptr<CurlHolder> Session::GetCurlHolder() {
    return curl_;
}

std::string Session::GetFullRequestUrl() {
    const std::string parametersContent = parameters_.GetContent(*curl_);
    return url_.str() + (parametersContent.empty() ? "" : "?") + parametersContent;
}

void Session::PrepareDelete() {
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPGET, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "DELETE");
    prepareCommon();
}

void Session::PrepareGet() {
    // In case there is a body or payload for this request, we create a custom GET-Request since a
    // GET-Request with body is based on the HTTP RFC **not** a leagal request.
    if (hasBodyOrPayload()) {
        curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "GET");
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, nullptr);
        curl_easy_setopt(curl_->handle, CURLOPT_HTTPGET, 1L);
    }
    prepareCommon();
}

void Session::PrepareHead() {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, nullptr);
    prepareCommon();
}

void Session::PrepareOptions() {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "OPTIONS");
    prepareCommon();
}

void Session::PreparePatch() {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "PATCH");
    prepareCommon();
}

void Session::PreparePost() {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);

    // In case there is no body or payload set it to an empty post:
    if (hasBodyOrPayload()) {
        curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, nullptr);
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDS, cbs_->readcb_.callback ? nullptr : "");
        curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "POST");
    }
    prepareCommon();
}

void Session::PreparePut() {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    if (!hasBodyOrPayload() && cbs_->readcb_.callback) {
        /**
         * Yes, this one has to be CURLOPT_POSTFIELDS even if we are performing a PUT request.
         * In case we don't set this one, performing a POST-request with PUT won't work.
         * It in theory this only enforces the usage of the readcallback for POST requests, but works here as well.
         **/
        curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDS, nullptr);
    }
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl_->handle, CURLOPT_RANGE, nullptr);
    prepareCommon();
}

void Session::PrepareDownload(std::ofstream& file) {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl_->handle, CURLOPT_WRITEFUNCTION, cpr::util::writeFileFunction);
    curl_easy_setopt(curl_->handle, CURLOPT_WRITEDATA, &file);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, nullptr);

    prepareCommonDownload();
}

void Session::PrepareDownload(const WriteCallback& write) {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, nullptr);

    SetWriteCallback(write);

    prepareCommonDownload();
}

Response Session::Complete(CURLcode curl_error) {
    curl_slist* raw_cookies{nullptr};
    curl_easy_getinfo(curl_->handle, CURLINFO_COOKIELIST, &raw_cookies);
    Cookies cookies = util::parseCookies(raw_cookies);
    curl_slist_free_all(raw_cookies);

    std::string errorMsg = curl_->error.data();
    return Response(curl_, std::move(response_string_), std::move(header_string_), std::move(cookies), Error(curl_error, std::move(errorMsg)));
}

Response Session::CompleteDownload(CURLcode curl_error) {
    if (!cbs_->headercb_.callback) {
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERFUNCTION, nullptr);
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERDATA, 0);
    }

    curl_slist* raw_cookies{nullptr};
    curl_easy_getinfo(curl_->handle, CURLINFO_COOKIELIST, &raw_cookies);
    Cookies cookies = util::parseCookies(raw_cookies);
    curl_slist_free_all(raw_cookies);
    std::string errorMsg = curl_->error.data();

    return Response(curl_, "", std::move(header_string_), std::move(cookies), Error(curl_error, std::move(errorMsg)));
}

void Session::AddInterceptor(const std::shared_ptr<Interceptor>& pinterceptor) {
    // Shall only add before first interceptor run
    assert(current_interceptor_ == interceptors_.end());
    interceptors_.push_back(pinterceptor);
    first_interceptor_ = interceptors_.begin();
}

Response Session::proceed() {
    prepareCommon();
    return makeRequest();
}

const std::optional<Response> Session::intercept() {
    if (current_interceptor_ == interceptors_.end()) {
        current_interceptor_ = first_interceptor_;
    } else {
        ++current_interceptor_;
    }

    if (current_interceptor_ != interceptors_.end()) {
        auto icpt = current_interceptor_;
        // Nested makeRequest() start at first_interceptor_, thus excluding previous interceptors.
        first_interceptor_ = current_interceptor_;
        ++first_interceptor_;

        const std::optional<Response> r = (*current_interceptor_)->intercept(*this);

        first_interceptor_ = icpt;

        return r;
    }
    return std::nullopt;
}

void Session::prepareBodyPayloadOrMultipart() const {
    // Either a body, multipart or a payload is allowed. Inverse function to RemoveContent()

    if (std::holds_alternative<cpr::Payload>(content_)) {
        const std::string payload = std::get<cpr::Payload>(content_).GetContent(*curl_);
        curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(payload.length()));
        curl_easy_setopt(curl_->handle, CURLOPT_COPYPOSTFIELDS, payload.c_str());
    } else if (std::holds_alternative<cpr::Body>(content_)) {
        const std::string& body = std::get<cpr::Body>(content_).str();
        curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(body.length()));
        curl_easy_setopt(curl_->handle, CURLOPT_COPYPOSTFIELDS, body.c_str());
    } else if (std::holds_alternative<cpr::BodyView>(content_)) {
        const std::string_view body = std::get<cpr::BodyView>(content_).str();
        curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(body.length()));
        // NOLINTNEXTLINE (bugprone-suspicious-stringview-data-usage)
        curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDS, body.data());
    } else if (std::holds_alternative<cpr::Multipart>(content_)) {
        // Make sure, we have a empty multipart to start with:
        if (curl_->multipart) {
            curl_mime_free(curl_->multipart);
        }
        curl_->multipart = curl_mime_init(curl_->handle);

        // Add all multipart pieces:
        const cpr::Multipart& multipart = std::get<cpr::Multipart>(content_);
        for (const Part& part : multipart.parts) {
            if (part.is_file) {
                for (const File& file : part.files) {
                    curl_mimepart* mimePart = curl_mime_addpart(curl_->multipart);
                    if (!part.content_type.empty()) {
                        curl_mime_type(mimePart, part.content_type.c_str());
                    }

                    curl_mime_filedata(mimePart, file.filepath.c_str());
                    curl_mime_name(mimePart, part.name.c_str());

                    if (file.hasOverridenFilename()) {
                        curl_mime_filename(mimePart, file.overriden_filename.c_str());
                    } else {
                        // NOLINTNEXTLINE (misc-include-cleaner)
                        curl_mime_filename(mimePart, fs::path(file.filepath).filename().string().c_str());
                    }
                }
            } else {
                curl_mimepart* mimePart = curl_mime_addpart(curl_->multipart);
                if (!part.content_type.empty()) {
                    curl_mime_type(mimePart, part.content_type.c_str());
                }
                if (part.is_buffer) {
                    // Do not use formdata, to prevent having to use reinterpreter_cast:
                    curl_mime_name(mimePart, part.name.c_str());
                    curl_mime_data(mimePart, part.data, part.datalen);
                    curl_mime_filename(mimePart, part.value.c_str());
                } else {
                    curl_mime_name(mimePart, part.name.c_str());
                    curl_mime_data(mimePart, part.value.c_str(), CURL_ZERO_TERMINATED);
                }
            }
        }

        curl_easy_setopt(curl_->handle, CURLOPT_MIMEPOST, curl_->multipart);
    }
}

[[nodiscard]] bool Session::hasBodyOrPayload() const {
    return std::holds_alternative<cpr::Body>(content_) || std::holds_alternative<cpr::BodyView>(content_) || std::holds_alternative<cpr::Payload>(content_);
}

// clang-format off
void Session::SetOption(const Resolve& resolve) { SetResolve(resolve); }
void Session::SetOption(const std::vector<Resolve>& resolves) { SetResolves(resolves); }
void Session::SetOption(const ReadCallback& read) { SetReadCallback(read); }
void Session::SetOption(const HeaderCallback& header) { SetHeaderCallback(header); }
void Session::SetOption(const WriteCallback& write) { SetWriteCallback(write); }
void Session::SetOption(const ProgressCallback& progress) { SetProgressCallback(progress); }
void Session::SetOption(const DebugCallback& debug) { SetDebugCallback(debug); }
void Session::SetOption(const Url& url) { SetUrl(url); }
void Session::SetOption(const Parameters& parameters) { SetParameters(parameters); }
void Session::SetOption(Parameters&& parameters) { SetParameters(std::move(parameters)); }
void Session::SetOption(const Header& header) { SetHeader(header); }
void Session::SetOption(const Timeout& timeout) { SetTimeout(timeout); }
void Session::SetOption(const ConnectTimeout& timeout) { SetConnectTimeout(timeout); }
void Session::SetOption(const Authentication& auth) { SetAuth(auth); }
void Session::SetOption(const LimitRate& limit_rate) { SetLimitRate(limit_rate); }
// Only supported with libcurl >= 7.61.0.
// As an alternative use SetHeader and add the token manually.
#if LIBCURL_VERSION_NUM >= 0x073D00
void Session::SetOption(const Bearer& auth) { SetBearer(auth); }
#endif
void Session::SetOption(const UserAgent& ua) { SetUserAgent(ua); }
void Session::SetOption(const Payload& payload) { SetPayload(payload); }
void Session::SetOption(Payload&& payload) { SetPayload(std::move(payload)); }
void Session::SetOption(const Proxies& proxies) { SetProxies(proxies); }
void Session::SetOption(Proxies&& proxies) { SetProxies(std::move(proxies)); }
void Session::SetOption(ProxyAuthentication&& proxy_auth) { SetProxyAuth(std::move(proxy_auth)); }
void Session::SetOption(const ProxyAuthentication& proxy_auth) { SetProxyAuth(proxy_auth); }
void Session::SetOption(const Multipart& multipart) { SetMultipart(multipart); }
void Session::SetOption(Multipart&& multipart) { SetMultipart(std::move(multipart)); }
void Session::SetOption(const Redirect& redirect) { SetRedirect(redirect); }
void Session::SetOption(const Cookies& cookies) { SetCookies(cookies); }
void Session::SetOption(const Body& body) { SetBody(body); }
void Session::SetOption(Body&& body) { SetBody(std::move(body)); }
// cppcheck-suppress passedByValue
void Session::SetOption(BodyView body) { SetBodyView(body); }
void Session::SetOption(const LowSpeed& low_speed) { SetLowSpeed(low_speed); }
void Session::SetOption(const VerifySsl& verify) { SetVerifySsl(verify); }
void Session::SetOption(const Verbose& verbose) { SetVerbose(verbose); }
void Session::SetOption(const UnixSocket& unix_socket) { SetUnixSocket(unix_socket); }
void Session::SetOption(const SslOptions& options) { SetSslOptions(options); }
void Session::SetOption(const Interface& iface) { SetInterface(iface); }
void Session::SetOption(const LocalPort& local_port) { SetLocalPort(local_port); }
void Session::SetOption(const LocalPortRange& local_port_range) { SetLocalPortRange(local_port_range); }
void Session::SetOption(const HttpVersion& version) { SetHttpVersion(version); }
void Session::SetOption(const Range& range) { SetRange(range); }
void Session::SetOption(const MultiRange& multi_range) { SetMultiRange(multi_range); }
void Session::SetOption(const ReserveSize& reserve_size) { SetReserveSize(reserve_size.size); }
void Session::SetOption(const AcceptEncoding& accept_encoding) { SetAcceptEncoding(accept_encoding); }
void Session::SetOption(AcceptEncoding&& accept_encoding) { SetAcceptEncoding(std::move(accept_encoding)); }
// clang-format on

void Session::SetCancellationParam(std::shared_ptr<std::atomic_bool> param) {
    cbs_->cancellationcb_ = CancellationCallback{std::move(param)};
    isCancellable = true;
#if LIBCURL_VERSION_NUM < 0x072000 // 7.32.0
    curl_easy_setopt(curl_->handle, CURLOPT_PROGRESSFUNCTION, cpr::util::progressUserFunction<CancellationCallback>);
    curl_easy_setopt(curl_->handle, CURLOPT_PROGRESSDATA, &cbs_->cancellationcb_);
#else
    curl_easy_setopt(curl_->handle, CURLOPT_XFERINFOFUNCTION, cpr::util::progressUserFunction<CancellationCallback>);
    curl_easy_setopt(curl_->handle, CURLOPT_XFERINFODATA, &cbs_->cancellationcb_);
#endif
    curl_easy_setopt(curl_->handle, CURLOPT_NOPROGRESS, 0L);
}
} // namespace cpr
