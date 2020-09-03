#include "cpr/session.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <string>

#include <curl/curl.h>

#include "cpr/curlholder.h"
#include "cpr/util.h"

namespace cpr {

const long ON = 1L;
const long OFF = 0L;

class Session::Impl {
  public:
    Impl();

    void SetUrl(const Url& url);
    void SetParameters(const Parameters& parameters);
    void SetParameters(Parameters&& parameters);
    void SetHeader(const Header& header);
    void SetTimeout(const Timeout& timeout);
    void SetConnectTimeout(const ConnectTimeout& timeout);
    void SetAuth(const Authentication& auth);
    void SetDigest(const Digest& auth);
    void SetUserAgent(const UserAgent& ua);
    void SetPayload(Payload&& payload);
    void SetPayload(const Payload& payload);
    void SetProxies(Proxies&& proxies);
    void SetProxies(const Proxies& proxies);
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
    void SetLimitRate(const LimitRate& limit_rate);
    void SetUnixSocket(const UnixSocket& unix_socket);
    void SetVerbose(const Verbose& verbose);
    void SetSslOptions(const SslOptions& options);

    Response Delete();
    Response Download(std::ofstream& file);
    Response Get();
    Response Head();
    Response Options();
    Response Patch();
    Response Post();
    Response Put();

  private:
    bool hasBodyOrPayload_{false};

    std::unique_ptr<CurlHolder> curl_;
    Url url_;
    Parameters parameters_;
    Proxies proxies_;

    Response makeDownloadRequest(CURL* curl, std::ofstream& file);
    Response makeRequest(CURL* curl);
    static void freeHolder(CurlHolder* holder);
};

Session::Impl::Impl() {
    curl_ = std::unique_ptr<CurlHolder>(new CurlHolder());
    CURL* curl = curl_->handle;
    if (curl) {
        // Set up some sensible defaults
        curl_version_info_data* version_info = curl_version_info(CURLVERSION_NOW);
        std::string version = "curl/" + std::string{version_info->version};
        curl_easy_setopt(curl, CURLOPT_USERAGENT, version.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_->error);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
#ifdef CPR_CURL_NOSIGNAL
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
#endif

#if LIBCURL_VERSION_MAJOR >= 7
#if LIBCURL_VERSION_MINOR >= 25
#if LIBCURL_VERSION_PATCH >= 0
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
#endif
#endif
#endif
    }
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

void Session::Impl::SetHeader(const Header& header) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_slist* chunk = nullptr;
        for (auto item = header.cbegin(); item != header.cend(); ++item) {
            std::string header_string = item->first;
            if (item->second.empty()) {
                header_string += ";";
            } else {
                header_string += ": " + item->second;
            }

            curl_slist* temp = curl_slist_append(chunk, header_string.c_str());
            if (temp) {
                chunk = temp;
            }
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        curl_slist_free_all(curl_->chunk);
        curl_->chunk = chunk;
    }
}

void Session::Impl::SetTimeout(const Timeout& timeout) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout.Milliseconds());
    }
}

void Session::Impl::SetConnectTimeout(const ConnectTimeout& timeout) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, timeout.Milliseconds());
    }
}

void Session::Impl::SetVerbose(const Verbose& verbose) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose.verbose ? ON : OFF);
    }
}

void Session::Impl::SetAuth(const Authentication& auth) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.GetAuthString());
    }
}

void Session::Impl::SetDigest(const Digest& auth) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.GetAuthString());
    }
}

void Session::Impl::SetUserAgent(const UserAgent& ua) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_USERAGENT, ua.c_str());
    }
}

void Session::Impl::SetPayload(Payload&& payload) {
    hasBodyOrPayload_ = true;
    CURL* curl = curl_->handle;
    const std::string content = payload.GetContent(*curl_);
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE,
                         static_cast<curl_off_t>(content.length()));
        curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, content.c_str());
    }
}

void Session::Impl::SetPayload(const Payload& payload) {
    hasBodyOrPayload_ = true;
    CURL* curl = curl_->handle;
    const std::string content = payload.GetContent(*curl_);
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE,
                         static_cast<curl_off_t>(content.length()));
        curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, std::move(content.c_str()));
    }
}

void Session::Impl::SetProxies(const Proxies& proxies) {
    proxies_ = proxies;
}

void Session::Impl::SetProxies(Proxies&& proxies) {
    proxies_ = std::move(proxies);
}

void Session::Impl::SetMultipart(Multipart&& multipart) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_httppost* formpost = nullptr;
        curl_httppost* lastptr = nullptr;

        for (auto& part : multipart.parts) {
            std::vector<curl_forms> formdata;
            formdata.push_back({CURLFORM_COPYNAME, part.name.c_str()});
            if (part.is_buffer) {
                formdata.push_back({CURLFORM_BUFFER, part.value.c_str()});
                formdata.push_back(
                        {CURLFORM_COPYCONTENTS, reinterpret_cast<const char*>(part.data)});
                formdata.push_back(
                        {CURLFORM_CONTENTSLENGTH, reinterpret_cast<const char*>(part.datalen)});
            } else if (part.is_file) {
                formdata.push_back({CURLFORM_FILE, part.value.c_str()});
            } else {
                formdata.push_back({CURLFORM_COPYCONTENTS, part.value.c_str()});
            }
            if (!part.content_type.empty()) {
                formdata.push_back({CURLFORM_CONTENTTYPE, part.content_type.c_str()});
            }
            formdata.push_back({CURLFORM_END, nullptr});
            curl_formadd(&formpost, &lastptr, CURLFORM_ARRAY, formdata.data(), CURLFORM_END);
        }
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        hasBodyOrPayload_ = true;

        curl_formfree(curl_->formpost);
        curl_->formpost = formpost;
    }
}

void Session::Impl::SetMultipart(const Multipart& multipart) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_httppost* formpost = nullptr;
        curl_httppost* lastptr = nullptr;

        for (auto& part : multipart.parts) {
            std::vector<curl_forms> formdata;
            formdata.push_back({CURLFORM_PTRNAME, part.name.c_str()});
            if (part.is_buffer) {
                formdata.push_back({CURLFORM_BUFFER, part.value.c_str()});
                formdata.push_back({CURLFORM_BUFFERPTR, reinterpret_cast<const char*>(part.data)});
                formdata.push_back(
                        {CURLFORM_BUFFERLENGTH, reinterpret_cast<const char*>(part.datalen)});
            } else if (part.is_file) {
                formdata.push_back({CURLFORM_FILE, part.value.c_str()});
            } else {
                formdata.push_back({CURLFORM_PTRCONTENTS, part.value.c_str()});
            }
            if (!part.content_type.empty()) {
                formdata.push_back({CURLFORM_CONTENTTYPE, part.content_type.c_str()});
            }
            formdata.push_back({CURLFORM_END, nullptr});
            curl_formadd(&formpost, &lastptr, CURLFORM_ARRAY, formdata.data(), CURLFORM_END);
        }
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        hasBodyOrPayload_ = true;

        curl_formfree(curl_->formpost);
        curl_->formpost = formpost;
    }
}

void Session::Impl::SetLimitRate(const LimitRate& limit_rate) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, limit_rate.downrate);
        curl_easy_setopt(curl, CURLOPT_MAX_SEND_SPEED_LARGE, limit_rate.uprate);
    }
}

void Session::Impl::SetNTLM(const NTLM& auth) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_NTLM);
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.GetAuthString());
    }
}

void Session::Impl::SetRedirect(const bool& redirect) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, std::int32_t(redirect));
    }
}

void Session::Impl::SetMaxRedirects(const MaxRedirects& max_redirects) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, max_redirects.number_of_redirects);
    }
}

void Session::Impl::SetCookies(const Cookies& cookies) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
        curl_easy_setopt(curl, CURLOPT_COOKIE, cookies.GetEncoded(*curl_).c_str());
    }
}

void Session::Impl::SetBody(Body&& body) {
    hasBodyOrPayload_ = true;
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, body.str().length());
        curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, body.c_str());
    }
}

void Session::Impl::SetBody(const Body& body) {
    hasBodyOrPayload_ = true;
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, body.str().length());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    }
}

void Session::Impl::SetLowSpeed(const LowSpeed& low_speed) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, low_speed.limit);
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, low_speed.time);
    }
}

void Session::Impl::SetVerifySsl(const VerifySsl& verify) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, verify ? ON : OFF);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, verify ? 2L : 0L);
    }
}

void Session::Impl::SetUnixSocket(const UnixSocket& unix_socket) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, unix_socket.GetUnixSocketString());
    }
}

void Session::Impl::SetSslOptions(const SslOptions& opts) {
    CURL* curl = curl_->handle;
    if (curl) {
        if (!opts.cert_file.empty()) {
            curl_easy_setopt(curl, CURLOPT_SSLCERT, opts.cert_file.c_str());
            if (!opts.cert_type.empty()) {
                curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, opts.cert_type.c_str());
            }
        }
        if (!opts.key_file.empty()) {
            curl_easy_setopt(curl, CURLOPT_SSLKEY, opts.key_file.c_str());
            if (!opts.key_type.empty()) {
                curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, opts.key_type.c_str());
            }
            if (!opts.key_pass.empty()) {
                curl_easy_setopt(curl, CURLOPT_KEYPASSWD, opts.key_pass.c_str());
            }
        }
#if SUPPORT_ALPN
        curl_easy_setopt(curl, CURLOPT_SSL_ENABLE_ALPN, opts.enable_alpn ? ON : OFF);
#endif
#if SUPPORT_NPN
        curl_easy_setopt(curl, CURLOPT_SSL_ENABLE_NPN, opts.enable_npn ? ON : OFF);
#endif
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, opts.verify_peer ? ON : OFF);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, opts.verify_host ? 2L : 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, opts.verify_status ? ON : OFF);
        curl_easy_setopt(curl, CURLOPT_SSLVERSION,
                         opts.ssl_version
#if SUPPORT_MAX_TLS_VERSION
                                 | opts.max_version
#endif
        );
        if (!opts.ca_info.empty()) {
            curl_easy_setopt(curl, CURLOPT_CAINFO, opts.ca_info.c_str());
        }
        if (!opts.ca_path.empty()) {
            curl_easy_setopt(curl, CURLOPT_CAPATH, opts.ca_path.c_str());
        }
        if (!opts.crl_file.empty()) {
            curl_easy_setopt(curl, CURLOPT_CRLFILE, opts.crl_file.c_str());
        }
        if (!opts.ciphers.empty()) {
            curl_easy_setopt(curl, CURLOPT_SSL_CIPHER_LIST, opts.ciphers.c_str());
        }
#if SUPPORT_TLSv13_CIPHERS
        if (!opts.tls13_ciphers.empty()) {
            curl_easy_setopt(curl, CURLOPT_TLS13_CIPHERS, opts.ciphers.c_str());
        }
#endif
#if SUPPORT_SESSIONID_CACHE
        curl_easy_setopt(curl, CURLOPT_SSL_SESSIONID_CACHE, opts.session_id_cache ? ON : OFF);
#endif
    }
}

Response Session::Impl::Delete() {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }

    return makeRequest(curl);
}

Response Session::Impl::Download(std::ofstream& file) {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    }

    return makeDownloadRequest(curl, file);
}

Response Session::Impl::Get() {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    }

    return makeRequest(curl);
}

Response Session::Impl::Head() {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, nullptr);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    }

    return makeRequest(curl);
}

Response Session::Impl::Options() {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
    }

    return makeRequest(curl);
}

Response Session::Impl::Patch() {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    }

    return makeRequest(curl);
}

Response Session::Impl::Post() {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

        // In case there is no body or payload set it to an empty post:
        if (!hasBodyOrPayload_) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
        }
    }

    return makeRequest(curl);
}

Response Session::Impl::Put() {
    CURL* curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    }

    return makeRequest(curl);
}

Response Session::Impl::makeDownloadRequest(CURL* curl, std::ofstream& file) {
    const std::string parametersContent = parameters_.GetContent(*curl_);
    if (!parametersContent.empty()) {
        Url new_url{url_ + "?" + parametersContent};
        curl_easy_setopt(curl, CURLOPT_URL, new_url.c_str());
    } else {
        curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
    }

    std::string protocol = url_.str().substr(0, url_.str().find(':'));
    if (proxies_.has(protocol)) {
        curl_easy_setopt(curl, CURLOPT_PROXY, proxies_[protocol].c_str());
    } else {
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
    }

    curl_->error[0] = '\0';

    std::string header_string;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cpr::util::downloadFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, cpr::util::writeFunction);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

    CURLcode curl_error = curl_easy_perform(curl);

    curl_slist* raw_cookies;
    curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &raw_cookies);
    Cookies cookies = util::parseCookies(raw_cookies);
    curl_slist_free_all(raw_cookies);

    return Response(curl, "", std::move(header_string), std::move(cookies),
                    Error(curl_error, curl_->error));
}

Response Session::Impl::makeRequest(CURL* curl) {
    const std::string parametersContent = parameters_.GetContent(*curl_);
    if (!parametersContent.empty()) {
        Url new_url{url_ + "?" + parametersContent};
        curl_easy_setopt(curl, CURLOPT_URL, new_url.c_str());
    } else {
        curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
    }

    std::string protocol = url_.str().substr(0, url_.str().find(':'));
    if (proxies_.has(protocol)) {
        curl_easy_setopt(curl, CURLOPT_PROXY, proxies_[protocol].c_str());
    } else {
        curl_easy_setopt(curl, CURLOPT_PROXY, nullptr);
    }

#if LIBCURL_VERSION_MAJOR >= 7
#if LIBCURL_VERSION_MINOR >= 21
    /* enable all supported built-in compressions */
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
#endif
#endif

    curl_->error[0] = '\0';

    std::string response_string;
    std::string header_string;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cpr::util::writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

    CURLcode curl_error = curl_easy_perform(curl);

    curl_slist* raw_cookies;
    curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &raw_cookies);
    Cookies cookies = util::parseCookies(raw_cookies);
    curl_slist_free_all(raw_cookies);

    // Reset the has no body property:
    hasBodyOrPayload_ = false;

    return Response(curl, std::move(response_string), std::move(header_string), std::move(cookies),
                    Error(curl_error, curl_->error));
}

// clang-format off
Session::Session() : pimpl_{ new Impl{} } {}
Session::Session(Session&& other) : pimpl_{ std::move(other.pimpl_) } {}
Session& Session::operator=(Session&& other) { pimpl_ = std::move(other.pimpl_); return *this; }
Session::~Session() {}
void Session::SetUrl(const Url& url) { pimpl_->SetUrl(url); }
void Session::SetParameters(const Parameters& parameters) { pimpl_->SetParameters(parameters); }
void Session::SetParameters(Parameters&& parameters) { pimpl_->SetParameters(std::move(parameters)); }
void Session::SetHeader(const Header& header) { pimpl_->SetHeader(header); }
void Session::SetTimeout(const Timeout& timeout) { pimpl_->SetTimeout(timeout); }
void Session::SetConnectTimeout(const ConnectTimeout& timeout) { pimpl_->SetConnectTimeout(timeout); }
void Session::SetAuth(const Authentication& auth) { pimpl_->SetAuth(auth); }
void Session::SetDigest(const Digest& auth) { pimpl_->SetDigest(auth); }
void Session::SetUserAgent(const UserAgent& ua) { pimpl_->SetUserAgent(ua); }
void Session::SetPayload(const Payload& payload) { pimpl_->SetPayload(payload); }
void Session::SetPayload(Payload&& payload) { pimpl_->SetPayload(std::move(payload)); }
void Session::SetProxies(const Proxies& proxies) { pimpl_->SetProxies(proxies); }
void Session::SetProxies(Proxies&& proxies) { pimpl_->SetProxies(std::move(proxies)); }
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
void Session::SetOption(const Url& url) { pimpl_->SetUrl(url); }
void Session::SetOption(const Parameters& parameters) { pimpl_->SetParameters(parameters); }
void Session::SetOption(Parameters&& parameters) { pimpl_->SetParameters(std::move(parameters)); }
void Session::SetOption(const Header& header) { pimpl_->SetHeader(header); }
void Session::SetOption(const Timeout& timeout) { pimpl_->SetTimeout(timeout); }
void Session::SetOption(const ConnectTimeout& timeout) { pimpl_->SetConnectTimeout(timeout); }
void Session::SetOption(const Authentication& auth) { pimpl_->SetAuth(auth); }
void Session::SetOption(const Digest& auth) { pimpl_->SetDigest(auth); }
void Session::SetOption(const UserAgent& ua) { pimpl_->SetUserAgent(ua); }
void Session::SetOption(const Payload& payload) { pimpl_->SetPayload(payload); }
void Session::SetOption(Payload&& payload) { pimpl_->SetPayload(std::move(payload)); }
void Session::SetOption(const Proxies& proxies) { pimpl_->SetProxies(proxies); }
void Session::SetOption(Proxies&& proxies) { pimpl_->SetProxies(std::move(proxies)); }
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
Response Session::Download(std::ofstream& file) { return pimpl_->Download(file); }
Response Session::Get() { return pimpl_->Get(); }
Response Session::Head() { return pimpl_->Head(); }
Response Session::Options() { return pimpl_->Options(); }
Response Session::Patch() { return pimpl_->Patch(); }
Response Session::Post() { return pimpl_->Post(); }
Response Session::Put() { return pimpl_->Put(); }
// clang-format on

} // namespace cpr
