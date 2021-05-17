#include "cpr/session.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <string>

#include <curl/curl.h>

#include "cpr/cprtypes.h"
#include "cpr/util.h"


namespace cpr {

// Ignored here since libcurl reqires a long:
// NOLINTNEXTLINE(google-runtime-int)
constexpr long ON = 1L;
// Ignored here since libcurl reqires a long:
// NOLINTNEXTLINE(google-runtime-int)
constexpr long OFF = 0L;

class Session::Impl {
  public:
    Impl();

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
    void SetReadCallback(const ReadCallback& read);
    void SetHeaderCallback(const HeaderCallback& header);
    void SetWriteCallback(const WriteCallback& write);
    void SetProgressCallback(const ProgressCallback& progress);
    void SetDebugCallback(const DebugCallback& debug);
    void SetLowSpeed(const LowSpeed& low_speed);
    void SetVerifySsl(const VerifySsl& verify);
    void SetLimitRate(const LimitRate& limit_rate);
    void SetUnixSocket(const UnixSocket& unix_socket);
    void SetVerbose(const Verbose& verbose);
    void SetSslOptions(const SslOptions& options);

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
    void SetHeaderInternal();
    bool hasBodyOrPayload_{false};

    std::shared_ptr<CurlHolder> curl_;
    Url url_;
    Parameters parameters_;
    Proxies proxies_;
    ProxyAuthentication proxyAuth_;
    Header header_;
    /**
     * Will be set by the read callback.
     * Ensures that the "Transfer-Encoding" is set to "chunked", if not overriden in header_.
     **/
    bool chunkedTransferEncoding{false};

    ReadCallback readcb_;
    HeaderCallback headercb_;
    WriteCallback writecb_;
    ProgressCallback progresscb_;
    DebugCallback debugcb_;
    std::string response_string_;
    std::string header_string_;

    Response makeDownloadRequest();
    Response makeRequest();
    void prepareCommon();
};

Session::Impl::Impl() : curl_(new CurlHolder()) {
    // Set up some sensible defaults
    curl_version_info_data* version_info = curl_version_info(CURLVERSION_NOW);
    std::string version = "curl/" + std::string{version_info->version};
    curl_easy_setopt(curl_->handle, CURLOPT_USERAGENT, version.c_str());
    curl_easy_setopt(curl_->handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_->handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_->handle, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl_->handle, CURLOPT_ERRORBUFFER, curl_->error.data());
    curl_easy_setopt(curl_->handle, CURLOPT_COOKIEFILE, "");
#ifdef CPR_CURL_NOSIGNAL
    curl_easy_setopt(curl_->handle, CURLOPT_NOSIGNAL, 1L);
#endif

#if LIBCURL_VERSION_MAJOR >= 7
#if LIBCURL_VERSION_MINOR >= 25
#if LIBCURL_VERSION_PATCH >= 0
    curl_easy_setopt(curl_->handle, CURLOPT_TCP_KEEPALIVE, 1L);
#endif
#endif
#endif
}

void Session::Impl::SetUrl(const Url& url) {
    url_ = url;
}

void Session::Impl::SetParameters(const Parameters& parameters) {
    parameters_ = parameters;
}

void Session::Impl::SetParameters(Parameters&& parameters) {
    parameters_ = std::move(parameters);
}

void Session::Impl::SetHeaderInternal() {
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
    if (chunkedTransferEncoding && header_.find("Transfer-Encoding") == header_.end()) {
        curl_slist* temp = curl_slist_append(chunk, "Transfer-Encoding:chunked");
        if (temp) {
            chunk = temp;
        }
    }

    curl_easy_setopt(curl_->handle, CURLOPT_HTTPHEADER, chunk);

    curl_slist_free_all(curl_->chunk);
    curl_->chunk = chunk;
}

void Session::Impl::SetHeader(const Header& header) {
    header_ = header;
}

void Session::Impl::UpdateHeader(const Header& header) {
    for (const std::pair<const std::string, std::string>& item : header) {
        header_[item.first] = item.second;
    }
}

void Session::Impl::SetTimeout(const Timeout& timeout) {
    curl_easy_setopt(curl_->handle, CURLOPT_TIMEOUT_MS, timeout.Milliseconds());
}

void Session::Impl::SetConnectTimeout(const ConnectTimeout& timeout) {
    curl_easy_setopt(curl_->handle, CURLOPT_CONNECTTIMEOUT_MS, timeout.Milliseconds());
}

void Session::Impl::SetVerbose(const Verbose& verbose) {
    curl_easy_setopt(curl_->handle, CURLOPT_VERBOSE, verbose.verbose ? ON : OFF);
}

void Session::Impl::SetAuth(const Authentication& auth) {
    // Ignore here since this has been defined by libcurl.
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl_->handle, CURLOPT_USERPWD, auth.GetAuthString());
}

// Only supported with libcurl >= 7.61.0.
// As an alternative use SetHeader and add the token manually.
#if LIBCURL_VERSION_NUM >= 0x073D00
void Session::Impl::SetBearer(const Bearer& token) {
    // Ignore here since this has been defined by libcurl.
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPAUTH, CURLAUTH_BEARER);
    curl_easy_setopt(curl_->handle, CURLOPT_XOAUTH2_BEARER, token.GetToken());
}
#endif

void Session::Impl::SetDigest(const Digest& auth) {
    // Ignore here since this has been defined by libcurl.
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
    curl_easy_setopt(curl_->handle, CURLOPT_USERPWD, auth.GetAuthString());
}

void Session::Impl::SetUserAgent(const UserAgent& ua) {
    curl_easy_setopt(curl_->handle, CURLOPT_USERAGENT, ua.c_str());
}

void Session::Impl::SetPayload(Payload&& payload) {
    hasBodyOrPayload_ = true;
    const std::string content = payload.GetContent(*curl_);
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE,
                     static_cast<curl_off_t>(content.length()));
    curl_easy_setopt(curl_->handle, CURLOPT_COPYPOSTFIELDS, content.c_str());
}

void Session::Impl::SetPayload(const Payload& payload) {
    hasBodyOrPayload_ = true;
    const std::string content = payload.GetContent(*curl_);
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE,
                     static_cast<curl_off_t>(content.length()));
    curl_easy_setopt(curl_->handle, CURLOPT_COPYPOSTFIELDS, content.c_str());
}

void Session::Impl::SetProxies(const Proxies& proxies) {
    proxies_ = proxies;
}

void Session::Impl::SetProxies(Proxies&& proxies) {
    proxies_ = std::move(proxies);
}

void Session::Impl::SetProxyAuth(ProxyAuthentication&& proxy_auth) {
    proxyAuth_ = std::move(proxy_auth);
}

void Session::Impl::SetProxyAuth(const ProxyAuthentication& proxy_auth) {
    proxyAuth_ = proxy_auth;
}

void Session::Impl::SetMultipart(Multipart&& multipart) {
    curl_httppost* formpost = nullptr;
    curl_httppost* lastptr = nullptr;

    for (const Part& part : multipart.parts) {
        std::vector<curl_forms> formdata;
        if (part.is_buffer) {
            // Do not use formdata, to prevent having to use reinterpreter_cast:
            curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, part.name.c_str(), CURLFORM_BUFFER,
                         part.value.c_str(), CURLFORM_BUFFERPTR, part.data, CURLFORM_BUFFERLENGTH,
                         part.datalen, CURLFORM_END);
        } else {
            formdata.push_back({CURLFORM_COPYNAME, part.name.c_str()});
            if (part.is_file) {
                formdata.push_back({CURLFORM_FILE, part.value.c_str()});
            } else {
                formdata.push_back({CURLFORM_COPYCONTENTS, part.value.c_str()});
            }
        }
        if (!part.content_type.empty()) {
            formdata.push_back({CURLFORM_CONTENTTYPE, part.content_type.c_str()});
        }

        formdata.push_back({CURLFORM_END, nullptr});
        curl_formadd(&formpost, &lastptr, CURLFORM_ARRAY, formdata.data(), CURLFORM_END);
    }
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPPOST, formpost);
    hasBodyOrPayload_ = true;

    curl_formfree(curl_->formpost);
    curl_->formpost = formpost;
}

void Session::Impl::SetMultipart(const Multipart& multipart) {
    curl_httppost* formpost = nullptr;
    curl_httppost* lastptr = nullptr;

    for (const Part& part : multipart.parts) {
        std::vector<curl_forms> formdata;
        if (part.is_buffer) {
            // Do not use formdata, to prevent having to use reinterpreter_cast:
            curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, part.name.c_str(), CURLFORM_BUFFER,
                         part.value.c_str(), CURLFORM_BUFFERPTR, part.data, CURLFORM_BUFFERLENGTH,
                         part.datalen, CURLFORM_END);
        } else {
            formdata.push_back({CURLFORM_COPYNAME, part.name.c_str()});
            if (part.is_file) {
                formdata.push_back({CURLFORM_FILE, part.value.c_str()});
            } else {
                formdata.push_back({CURLFORM_COPYCONTENTS, part.value.c_str()});
            }
        }
        if (!part.content_type.empty()) {
            formdata.push_back({CURLFORM_CONTENTTYPE, part.content_type.c_str()});
        }

        formdata.push_back({CURLFORM_END, nullptr});
        curl_formadd(&formpost, &lastptr, CURLFORM_ARRAY, formdata.data(), CURLFORM_END);
    }
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPPOST, formpost);
    hasBodyOrPayload_ = true;

    curl_formfree(curl_->formpost);
    curl_->formpost = formpost;
}

void Session::Impl::SetLimitRate(const LimitRate& limit_rate) {
    curl_easy_setopt(curl_->handle, CURLOPT_MAX_RECV_SPEED_LARGE, limit_rate.downrate);
    curl_easy_setopt(curl_->handle, CURLOPT_MAX_SEND_SPEED_LARGE, limit_rate.uprate);
}

void Session::Impl::SetNTLM(const NTLM& auth) {
    // Ignore here since this has been defined by libcurl.
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPAUTH, CURLAUTH_NTLM);
    curl_easy_setopt(curl_->handle, CURLOPT_USERPWD, auth.GetAuthString());
}

void Session::Impl::SetRedirect(const bool& redirect) {
    curl_easy_setopt(curl_->handle, CURLOPT_FOLLOWLOCATION, std::int32_t(redirect));
}

void Session::Impl::SetMaxRedirects(const MaxRedirects& max_redirects) {
    curl_easy_setopt(curl_->handle, CURLOPT_MAXREDIRS, max_redirects.number_of_redirects);
}

void Session::Impl::SetCookies(const Cookies& cookies) {
    curl_easy_setopt(curl_->handle, CURLOPT_COOKIELIST, "ALL");
    curl_easy_setopt(curl_->handle, CURLOPT_COOKIE, cookies.GetEncoded(*curl_).c_str());
}

void Session::Impl::SetBody(Body&& body) {
    hasBodyOrPayload_ = true;
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE,
                     static_cast<curl_off_t>(body.str().length()));
    curl_easy_setopt(curl_->handle, CURLOPT_COPYPOSTFIELDS, body.c_str());
}

void Session::Impl::SetBody(const Body& body) {
    hasBodyOrPayload_ = true;
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE,
                     static_cast<curl_off_t>(body.str().length()));
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDS, body.c_str());
}

void Session::Impl::SetReadCallback(const ReadCallback& read) {
    readcb_ = read;
    curl_easy_setopt(curl_->handle, CURLOPT_INFILESIZE_LARGE, read.size);
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE, read.size);
    curl_easy_setopt(curl_->handle, CURLOPT_READFUNCTION, cpr::util::readUserFunction);
    curl_easy_setopt(curl_->handle, CURLOPT_READDATA, &readcb_);
    chunkedTransferEncoding = read.size == -1;
}

void Session::Impl::SetHeaderCallback(const HeaderCallback& header) {
    curl_easy_setopt(curl_->handle, CURLOPT_HEADERFUNCTION, cpr::util::headerUserFunction);
    headercb_ = header;
    curl_easy_setopt(curl_->handle, CURLOPT_HEADERDATA, &headercb_);
}

void Session::Impl::SetWriteCallback(const WriteCallback& write) {
    curl_easy_setopt(curl_->handle, CURLOPT_WRITEFUNCTION, cpr::util::writeUserFunction);
    writecb_ = write;
    curl_easy_setopt(curl_->handle, CURLOPT_WRITEDATA, &writecb_);
}

void Session::Impl::SetProgressCallback(const ProgressCallback& progress) {
    progresscb_ = progress;
#if LIBCURL_VERSION_NUM < 0x072000
    curl_easy_setopt(curl_->handle, CURLOPT_PROGRESSFUNCTION, cpr::util::progressUserFunction);
    curl_easy_setopt(curl_->handle, CURLOPT_PROGRESSDATA, &progresscb_);
#else
    curl_easy_setopt(curl_->handle, CURLOPT_XFERINFOFUNCTION, cpr::util::progressUserFunction);
    curl_easy_setopt(curl_->handle, CURLOPT_XFERINFODATA, &progresscb_);
#endif
    curl_easy_setopt(curl_->handle, CURLOPT_NOPROGRESS, 0L);
}

void Session::Impl::SetDebugCallback(const DebugCallback& debug) {
    curl_easy_setopt(curl_->handle, CURLOPT_DEBUGFUNCTION, cpr::util::debugUserFunction);
    debugcb_ = debug;
    curl_easy_setopt(curl_->handle, CURLOPT_DEBUGDATA, &debugcb_);
    curl_easy_setopt(curl_->handle, CURLOPT_VERBOSE, 1L);
}

void Session::Impl::SetLowSpeed(const LowSpeed& low_speed) {
    curl_easy_setopt(curl_->handle, CURLOPT_LOW_SPEED_LIMIT, low_speed.limit);
    curl_easy_setopt(curl_->handle, CURLOPT_LOW_SPEED_TIME, low_speed.time);
}

void Session::Impl::SetVerifySsl(const VerifySsl& verify) {
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_VERIFYPEER, verify ? ON : OFF);
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_VERIFYHOST, verify ? 2L : 0L);
}

void Session::Impl::SetUnixSocket(const UnixSocket& unix_socket) {
    curl_easy_setopt(curl_->handle, CURLOPT_UNIX_SOCKET_PATH, unix_socket.GetUnixSocketString());
}

void Session::Impl::SetSslOptions(const SslOptions& options) {
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
#if LIBCURL_VERSION_NUM >= 0x072900
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_VERIFYSTATUS, options.verify_status ? ON : OFF);
#endif

    int maxTlsVersion = options.ssl_version;
#if SUPPORT_MAX_TLS_VERSION
    maxTlsVersion |= options.max_version;
#endif

    curl_easy_setopt(curl_->handle, CURLOPT_SSLVERSION,
                     // Ignore here since this has been defined by libcurl.
                     maxTlsVersion);
#if SUPPORT_SSL_NO_REVOKE
    if (options.ssl_no_revoke) {
        curl_easy_setopt(curl_->handle, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NO_REVOKE);
    }
#endif
    if (!options.ca_info.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_CAINFO, options.ca_info.c_str());
    }
    if (!options.ca_path.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_CAPATH, options.ca_path.c_str());
    }
    if (!options.crl_file.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_CRLFILE, options.crl_file.c_str());
    }
    if (!options.ciphers.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_SSL_CIPHER_LIST, options.ciphers.c_str());
    }
#if SUPPORT_TLSv13_CIPHERS
    if (!options.tls13_ciphers.empty()) {
        curl_easy_setopt(curl_->handle, CURLOPT_TLS13_CIPHERS, options.ciphers.c_str());
    }
#endif
#if SUPPORT_SESSIONID_CACHE
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_SESSIONID_CACHE,
                     options.session_id_cache ? ON : OFF);
#endif
}

void Session::Impl::PrepareDelete() {
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPGET, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "DELETE");
    prepareCommon();
}

Response Session::Impl::Delete() {
    PrepareDelete();
    return makeRequest();
}

Response Session::Impl::Download(const WriteCallback& write) {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "GET");

    SetWriteCallback(write);

    return makeDownloadRequest();
}

Response Session::Impl::Download(std::ofstream& file) {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl_->handle, CURLOPT_WRITEFUNCTION, cpr::util::writeFileFunction);
    curl_easy_setopt(curl_->handle, CURLOPT_WRITEDATA, &file);

    return makeDownloadRequest();
}

void Session::Impl::PrepareGet() {
    // In case there is a body or payload for this request, we create a custom GET-Request since a
    // GET-Request with body is based on the HTTP RFC **not** a leagal request.
    if (hasBodyOrPayload_) {
        curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "GET");
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, nullptr);
        curl_easy_setopt(curl_->handle, CURLOPT_HTTPGET, 1L);
    }
	prepareCommon();
}

Response Session::Impl::Get() {
    PrepareGet();
    return makeRequest();
}

void Session::Impl::PrepareHead() {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, nullptr);
	 prepareCommon();
}

Response Session::Impl::Head() {
    PrepareHead();
    return makeRequest();
}

void Session::Impl::PrepareOptions() {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "OPTIONS");
    prepareCommon();
}

Response Session::Impl::Options() {
    PrepareOptions();
    return makeRequest();
}

void Session::Impl::PreparePatch() {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "PATCH");
    prepareCommon();
}

Response Session::Impl::Patch() {
    PreparePatch();
    return makeRequest();
}

void Session::Impl::PreparePost() {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);

    // In case there is no body or payload set it to an empty post:
    if (hasBodyOrPayload_) {
        curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, nullptr);
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDS, readcb_.callback ? nullptr : "");
        curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "POST");
    }
    prepareCommon();
}

Response Session::Impl::Post() {
    PreparePost();
    return makeRequest();
}

void Session::Impl::PreparePut() {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "PUT");
	prepareCommon();
}

Response Session::Impl::Put() {
    PreparePut();
    return makeRequest();
}

std::shared_ptr<CurlHolder> Session::Impl::GetCurlHolder() {
    return curl_;
}

Response Session::Impl::makeDownloadRequest() {
    assert(curl_->handle);
    const std::string parametersContent = parameters_.GetContent(*curl_);
    if (!parametersContent.empty()) {
        Url new_url{url_ + "?" + parametersContent};
        curl_easy_setopt(curl_->handle, CURLOPT_URL, new_url.c_str());
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_URL, url_.c_str());
    }

    std::string protocol = url_.str().substr(0, url_.str().find(':'));
    if (proxies_.has(protocol)) {
        curl_easy_setopt(curl_->handle, CURLOPT_PROXY, proxies_[protocol].c_str());
        if (proxyAuth_.has(protocol)) {
            curl_easy_setopt(curl_->handle, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
            curl_easy_setopt(curl_->handle, CURLOPT_PROXYUSERPWD, proxyAuth_[protocol]);
        }
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_PROXY, "");
    }

    curl_->error[0] = '\0';

    std::string header_string;
    if (headercb_.callback) {
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERFUNCTION, cpr::util::headerUserFunction);
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERDATA, &headercb_);
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERFUNCTION, cpr::util::writeFunction);
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERDATA, &header_string);
    }

    CURLcode curl_error = curl_easy_perform(curl_->handle);

    curl_slist* raw_cookies{nullptr};
    curl_easy_getinfo(curl_->handle, CURLINFO_COOKIELIST, &raw_cookies);
    Cookies cookies = util::parseCookies(raw_cookies);
    curl_slist_free_all(raw_cookies);
    std::string errorMsg = curl_->error.data();

    return Response(curl_, "", std::move(header_string), std::move(cookies),
                    Error(curl_error, std::move(errorMsg)));
}

void Session::Impl::prepareCommon() {
    assert(curl_->handle);

    // Set Header:
    SetHeaderInternal();

    const std::string parametersContent = parameters_.GetContent(*curl_);
    if (!parametersContent.empty()) {
        Url new_url{url_ + "?" + parametersContent};
        curl_easy_setopt(curl_->handle, CURLOPT_URL, new_url.c_str());
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_URL, url_.c_str());
    }

    // Proxy:
    std::string protocol = url_.str().substr(0, url_.str().find(':'));
    if (proxies_.has(protocol)) {
        curl_easy_setopt(curl_->handle, CURLOPT_PROXY, proxies_[protocol].c_str());
        if (proxyAuth_.has(protocol)) {
            curl_easy_setopt(curl_->handle, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
            curl_easy_setopt(curl_->handle, CURLOPT_PROXYUSERPWD, proxyAuth_[protocol]);
        }
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_PROXY, nullptr);
    }

#if LIBCURL_VERSION_MAJOR >= 7
#if LIBCURL_VERSION_MINOR >= 21
    /* enable all supported built-in compressions */
    curl_easy_setopt(curl_->handle, CURLOPT_ACCEPT_ENCODING, "");
#endif
#endif

#if LIBCURL_VERSION_MAJOR >= 7
#if LIBCURL_VERSION_MINOR >= 71
    // Fix loading certs from Windows cert store when using OpenSSL:
    curl_easy_setopt(curl_->handle, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
#endif
#endif

    curl_->error[0] = '\0';

    response_string_.clear();
    header_string_.clear();
    if (!this->writecb_.callback) {
        curl_easy_setopt(curl_->handle, CURLOPT_WRITEFUNCTION, cpr::util::writeFunction);
        curl_easy_setopt(curl_->handle, CURLOPT_WRITEDATA, &response_string_);
    }
    if (!this->headercb_.callback) {
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERFUNCTION, cpr::util::writeFunction);
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERDATA, &header_string_);
    }

    // Enable so we are able to retrive certificate information:
    curl_easy_setopt(curl_->handle, CURLOPT_CERTINFO, 1L);
}

Response Session::Impl::makeRequest()
{
    CURLcode curl_error = curl_easy_perform(curl_->handle);
    return Complete(curl_error);
}

Response Session::Impl::Complete(CURLcode curl_error)
{
    curl_slist* raw_cookies{nullptr};
    curl_easy_getinfo(curl_->handle, CURLINFO_COOKIELIST, &raw_cookies);
    Cookies cookies = util::parseCookies(raw_cookies);
    curl_slist_free_all(raw_cookies);

    // Reset the has no body property:
    hasBodyOrPayload_ = false;

    std::string errorMsg = curl_->error.data();
    return Response(curl_, std::move(response_string_), std::move(header_string_), std::move(cookies),
                    Error(curl_error, std::move(errorMsg)));
}

// clang-format off
Session::Session() : pimpl_(new Impl()) {}
Session::Session(Session&& /*old*/) noexcept = default;
Session::~Session() = default;
Session& Session::operator=(Session&& old) noexcept = default;
void Session::SetReadCallback(const ReadCallback& read) { pimpl_->SetReadCallback(read); }
void Session::SetHeaderCallback(const HeaderCallback& header) { pimpl_->SetHeaderCallback(header); }
void Session::SetWriteCallback(const WriteCallback& write) { pimpl_->SetWriteCallback(write); }
void Session::SetProgressCallback(const ProgressCallback& progress) { pimpl_->SetProgressCallback(progress); }
void Session::SetDebugCallback(const DebugCallback& debug) { pimpl_->SetDebugCallback(debug); }
void Session::SetUrl(const Url& url) { pimpl_->SetUrl(url); }
void Session::SetParameters(const Parameters& parameters) { pimpl_->SetParameters(parameters); }
void Session::SetParameters(Parameters&& parameters) { pimpl_->SetParameters(std::move(parameters)); }
void Session::SetHeader(const Header& header) { pimpl_->SetHeader(header); }
void Session::UpdateHeader(const Header& header) { pimpl_->UpdateHeader(header); }
void Session::SetTimeout(const Timeout& timeout) { pimpl_->SetTimeout(timeout); }
void Session::SetConnectTimeout(const ConnectTimeout& timeout) { pimpl_->SetConnectTimeout(timeout); }
void Session::SetAuth(const Authentication& auth) { pimpl_->SetAuth(auth); }
void Session::SetDigest(const Digest& auth) { pimpl_->SetDigest(auth); }
void Session::SetUserAgent(const UserAgent& ua) { pimpl_->SetUserAgent(ua); }
void Session::SetPayload(const Payload& payload) { pimpl_->SetPayload(payload); }
void Session::SetPayload(Payload&& payload) { pimpl_->SetPayload(std::move(payload)); }
void Session::SetProxies(const Proxies& proxies) { pimpl_->SetProxies(proxies); }
void Session::SetProxies(Proxies&& proxies) { pimpl_->SetProxies(std::move(proxies)); }
void Session::SetProxyAuth(ProxyAuthentication&& proxy_auth) { pimpl_->SetProxyAuth(std::move(proxy_auth)); }
void Session::SetProxyAuth(const ProxyAuthentication& proxy_auth) { pimpl_->SetProxyAuth(proxy_auth); }
void Session::SetMultipart(const Multipart& multipart) { pimpl_->SetMultipart(multipart); }
void Session::SetMultipart(Multipart&& multipart) { pimpl_->SetMultipart(std::move(multipart)); }
void Session::SetNTLM(const NTLM& auth) { pimpl_->SetNTLM(auth); }
void Session::SetRedirect(const bool& redirect) { pimpl_->SetRedirect(redirect); }
void Session::SetMaxRedirects(const MaxRedirects& max_redirects) { pimpl_->SetMaxRedirects(max_redirects); }
void Session::SetCookies(const Cookies& cookies) { pimpl_->SetCookies(cookies); }
void Session::SetBody(const Body& body) { pimpl_->SetBody(body); }
void Session::SetBody(Body&& body) { pimpl_->SetBody(std::move(body)); }
void Session::SetLowSpeed(const LowSpeed& low_speed) { pimpl_->SetLowSpeed(low_speed); }
void Session::SetVerifySsl(const VerifySsl& verify) { pimpl_->SetVerifySsl(verify); }
void Session::SetUnixSocket(const UnixSocket& unix_socket) { pimpl_->SetUnixSocket(unix_socket); }
void Session::SetSslOptions(const SslOptions& options) { pimpl_->SetSslOptions(options); }
void Session::SetVerbose(const Verbose& verbose) { pimpl_->SetVerbose(verbose); }
void Session::SetOption(const ReadCallback& read) { pimpl_->SetReadCallback(read); }
void Session::SetOption(const HeaderCallback& header) { pimpl_->SetHeaderCallback(header); }
void Session::SetOption(const WriteCallback& write) { pimpl_->SetWriteCallback(write); }
void Session::SetOption(const ProgressCallback& progress) { pimpl_->SetProgressCallback(progress); }
void Session::SetOption(const DebugCallback& debug) { pimpl_->SetDebugCallback(debug); }
void Session::SetOption(const Url& url) { pimpl_->SetUrl(url); }
void Session::SetOption(const Parameters& parameters) { pimpl_->SetParameters(parameters); }
void Session::SetOption(Parameters&& parameters) { pimpl_->SetParameters(std::move(parameters)); }
void Session::SetOption(const Header& header) { pimpl_->SetHeader(header); }
void Session::SetOption(const Timeout& timeout) { pimpl_->SetTimeout(timeout); }
void Session::SetOption(const ConnectTimeout& timeout) { pimpl_->SetConnectTimeout(timeout); }
void Session::SetOption(const Authentication& auth) { pimpl_->SetAuth(auth); }
void Session::SetOption(const LimitRate& limit_rate) { pimpl_->SetLimitRate(limit_rate); }
// Only supported with libcurl >= 7.61.0.
// As an alternative use SetHeader and add the token manually.
#if LIBCURL_VERSION_NUM >= 0x073D00
void Session::SetOption(const Bearer& auth) { pimpl_->SetBearer(auth); }
#endif
void Session::SetOption(const Digest& auth) { pimpl_->SetDigest(auth); }
void Session::SetOption(const UserAgent& ua) { pimpl_->SetUserAgent(ua); }
void Session::SetOption(const Payload& payload) { pimpl_->SetPayload(payload); }
void Session::SetOption(Payload&& payload) { pimpl_->SetPayload(std::move(payload)); }
void Session::SetOption(const Proxies& proxies) { pimpl_->SetProxies(proxies); }
void Session::SetOption(Proxies&& proxies) { pimpl_->SetProxies(std::move(proxies)); }
void Session::SetOption(ProxyAuthentication&& proxy_auth) { pimpl_->SetProxyAuth(std::move(proxy_auth)); }
void Session::SetOption(const ProxyAuthentication& proxy_auth) { pimpl_->SetProxyAuth(proxy_auth); }
void Session::SetOption(const Multipart& multipart) { pimpl_->SetMultipart(multipart); }
void Session::SetOption(Multipart&& multipart) { pimpl_->SetMultipart(std::move(multipart)); }
void Session::SetOption(const NTLM& auth) { pimpl_->SetNTLM(auth); }
void Session::SetOption(const bool& redirect) { pimpl_->SetRedirect(redirect); }
void Session::SetOption(const MaxRedirects& max_redirects) { pimpl_->SetMaxRedirects(max_redirects); }
void Session::SetOption(const Cookies& cookies) { pimpl_->SetCookies(cookies); }
void Session::SetOption(const Body& body) { pimpl_->SetBody(body); }
void Session::SetOption(Body&& body) { pimpl_->SetBody(std::move(body)); }
void Session::SetOption(const LowSpeed& low_speed) { pimpl_->SetLowSpeed(low_speed); }
void Session::SetOption(const VerifySsl& verify) { pimpl_->SetVerifySsl(verify); }
void Session::SetOption(const Verbose& verbose) { pimpl_->SetVerbose(verbose); }
void Session::SetOption(const UnixSocket& unix_socket) { pimpl_->SetUnixSocket(unix_socket); }
void Session::SetOption(const SslOptions& options) { pimpl_->SetSslOptions(options); }

Response Session::Delete() { return pimpl_->Delete(); }
Response Session::Download(const WriteCallback& write) { return pimpl_->Download(write); }
Response Session::Download(std::ofstream& file) { return pimpl_->Download(file); }
Response Session::Get() { return pimpl_->Get(); }
Response Session::Head() { return pimpl_->Head(); }
Response Session::Options() { return pimpl_->Options(); }
Response Session::Patch() { return pimpl_->Patch(); }
Response Session::Post() { return pimpl_->Post(); }
Response Session::Put() { return pimpl_->Put(); }

std::shared_ptr<CurlHolder> Session::GetCurlHolder() { return pimpl_->GetCurlHolder(); }

void Session::PrepareDelete() { return pimpl_->PrepareDelete(); }
void Session::PrepareGet() { return pimpl_->PrepareGet(); }
void Session::PrepareHead() { return pimpl_->PrepareHead(); }
void Session::PrepareOptions() { return pimpl_->PrepareOptions(); }
void Session::PreparePatch() { return pimpl_->PreparePatch(); }
void Session::PreparePost() { return pimpl_->PreparePost(); }
void Session::PreparePut() { return pimpl_->PreparePut(); }
Response Session::Complete( CURLcode curl_error ) { return pimpl_->Complete(curl_error); }

// clang-format on
} // namespace cpr
