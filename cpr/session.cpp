#include "cpr/session.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>

#include <curl/curl.h>

#include "cpr/async.h"
#include "cpr/cprtypes.h"
#include "cpr/interceptor.h"
#include "cpr/util.h"

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

void Session::SetHeaderInternal() {
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

    curl_easy_setopt(curl_->handle, CURLOPT_HTTPHEADER, chunk);

    curl_slist_free_all(curl_->chunk);
    curl_->chunk = chunk;
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
    std::string version = "curl/" + std::string{version_info->version};
    curl_easy_setopt(curl_->handle, CURLOPT_USERAGENT, version.c_str());
    SetRedirect(Redirect());
    curl_easy_setopt(curl_->handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_->handle, CURLOPT_ERRORBUFFER, curl_->error.data());
    curl_easy_setopt(curl_->handle, CURLOPT_COOKIEFILE, "");
#ifdef CPR_CURL_NOSIGNAL
    curl_easy_setopt(curl_->handle, CURLOPT_NOSIGNAL, 1L);
#endif

#if LIBCURL_VERSION_NUM >= 0x071900
    curl_easy_setopt(curl_->handle, CURLOPT_TCP_KEEPALIVE, 1L);
#endif
}

Response Session::makeDownloadRequest() {
    assert(curl_->handle);

    if (!interceptors_.empty()) {
        std::shared_ptr<Interceptor> interceptor = interceptors_.front();
        interceptors_.pop();
        return interceptor->intercept(*this);
    }

    // Set Header:
    SetHeaderInternal();

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

    if (!headercb_.callback) {
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERFUNCTION, nullptr);
        curl_easy_setopt(curl_->handle, CURLOPT_HEADERDATA, 0);
    }

    curl_slist* raw_cookies{nullptr};
    curl_easy_getinfo(curl_->handle, CURLINFO_COOKIELIST, &raw_cookies);
    Cookies cookies = util::parseCookies(raw_cookies);
    curl_slist_free_all(raw_cookies);
    std::string errorMsg = curl_->error.data();

    return Response(curl_, "", std::move(header_string), std::move(cookies), Error(curl_error, std::move(errorMsg)));
}

void Session::prepareCommon() {
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
    }

#if LIBCURL_VERSION_MAJOR >= 7
#if LIBCURL_VERSION_MINOR >= 21
    if (acceptEncoding_.empty()) {
        /* enable all supported built-in compressions */
        curl_easy_setopt(curl_->handle, CURLOPT_ACCEPT_ENCODING, "");
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_ACCEPT_ENCODING, acceptEncoding_.getString().c_str());
    }
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
    if (response_string_reserve_size_ > 0) {
        response_string_.reserve(response_string_reserve_size_);
    }
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

Response Session::makeRequest() {
    if (!interceptors_.empty()) {
        // At least one interceptor exists -> Execute its intercept function
        std::shared_ptr<Interceptor> interceptor = interceptors_.front();
        interceptors_.pop();
        return interceptor->intercept(*this);
    }

    CURLcode curl_error = curl_easy_perform(curl_->handle);
    return Complete(curl_error);
}

void Session::SetLimitRate(const LimitRate& limit_rate) {
    curl_easy_setopt(curl_->handle, CURLOPT_MAX_RECV_SPEED_LARGE, limit_rate.downrate);
    curl_easy_setopt(curl_->handle, CURLOPT_MAX_SEND_SPEED_LARGE, limit_rate.uprate);
}

void Session::SetReadCallback(const ReadCallback& read) {
    readcb_ = read;
    curl_easy_setopt(curl_->handle, CURLOPT_INFILESIZE_LARGE, read.size);
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE, read.size);
    curl_easy_setopt(curl_->handle, CURLOPT_READFUNCTION, cpr::util::readUserFunction);
    curl_easy_setopt(curl_->handle, CURLOPT_READDATA, &readcb_);
    chunkedTransferEncoding_ = read.size == -1;
}

void Session::SetHeaderCallback(const HeaderCallback& header) {
    curl_easy_setopt(curl_->handle, CURLOPT_HEADERFUNCTION, cpr::util::headerUserFunction);
    headercb_ = header;
    curl_easy_setopt(curl_->handle, CURLOPT_HEADERDATA, &headercb_);
}

void Session::SetWriteCallback(const WriteCallback& write) {
    curl_easy_setopt(curl_->handle, CURLOPT_WRITEFUNCTION, cpr::util::writeUserFunction);
    writecb_ = write;
    curl_easy_setopt(curl_->handle, CURLOPT_WRITEDATA, &writecb_);
}

void Session::SetProgressCallback(const ProgressCallback& progress) {
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

void Session::SetDebugCallback(const DebugCallback& debug) {
    curl_easy_setopt(curl_->handle, CURLOPT_DEBUGFUNCTION, cpr::util::debugUserFunction);
    debugcb_ = debug;
    curl_easy_setopt(curl_->handle, CURLOPT_DEBUGDATA, &debugcb_);
    curl_easy_setopt(curl_->handle, CURLOPT_VERBOSE, 1L);
}

void Session::SetUrl(const Url& url) {
    url_ = url;
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
    }
}

void Session::SetUserAgent(const UserAgent& ua) {
    curl_easy_setopt(curl_->handle, CURLOPT_USERAGENT, ua.c_str());
}

void Session::SetPayload(const Payload& payload) {
    hasBodyOrPayload_ = true;
    const std::string content = payload.GetContent(*curl_);
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(content.length()));
    curl_easy_setopt(curl_->handle, CURLOPT_COPYPOSTFIELDS, content.c_str());
}

void Session::SetPayload(Payload&& payload) {
    hasBodyOrPayload_ = true;
    const std::string content = payload.GetContent(*curl_);
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(content.length()));
    curl_easy_setopt(curl_->handle, CURLOPT_COPYPOSTFIELDS, content.c_str());
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
    curl_httppost* formpost = nullptr;
    curl_httppost* lastptr = nullptr;

    for (const Part& part : multipart.parts) {
        std::vector<curl_forms> formdata;
        if (part.is_buffer) {
            // Do not use formdata, to prevent having to use reinterpreter_cast:
            curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, part.name.c_str(), CURLFORM_BUFFER, part.value.c_str(), CURLFORM_BUFFERPTR, part.data, CURLFORM_BUFFERLENGTH, part.datalen, CURLFORM_END);
        } else {
            formdata.push_back({CURLFORM_COPYNAME, part.name.c_str()});
            if (part.is_file) {
                formdata.push_back({CURLFORM_FILE, part.value.c_str()});
                if (part.has_filename) {
                    formdata.push_back({CURLFORM_FILENAME, part.filename.c_str()});
                } else {
                    formdata.push_back({CURLFORM_FILENAME, part.value.c_str()});
                }
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

void Session::SetMultipart(Multipart&& multipart) {
    curl_httppost* formpost = nullptr;
    curl_httppost* lastptr = nullptr;

    for (const Part& part : multipart.parts) {
        std::vector<curl_forms> formdata;
        if (part.is_buffer) {
            // Do not use formdata, to prevent having to use reinterpreter_cast:
            curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, part.name.c_str(), CURLFORM_BUFFER, part.value.c_str(), CURLFORM_BUFFERPTR, part.data, CURLFORM_BUFFERLENGTH, part.datalen, CURLFORM_END);
        } else {
            formdata.push_back({CURLFORM_COPYNAME, part.name.c_str()});
            if (part.is_file) {
                formdata.push_back({CURLFORM_FILE, part.value.c_str()});
                if (part.has_filename) {
                    formdata.push_back({CURLFORM_FILENAME, part.filename.c_str()});
                } else {
                    formdata.push_back({CURLFORM_FILENAME, part.value.c_str()});
                }
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
    hasBodyOrPayload_ = true;
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(body.str().length()));
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDS, body.c_str());
}

void Session::SetBody(Body&& body) {
    hasBodyOrPayload_ = true;
    curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(body.str().length()));
    curl_easy_setopt(curl_->handle, CURLOPT_COPYPOSTFIELDS, body.c_str());
}

void Session::SetLowSpeed(const LowSpeed& low_speed) {
    curl_easy_setopt(curl_->handle, CURLOPT_LOW_SPEED_LIMIT, low_speed.limit);
    curl_easy_setopt(curl_->handle, CURLOPT_LOW_SPEED_TIME, low_speed.time);
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
        curl_easy_setopt(curl_->handle, CURLOPT_TLS13_CIPHERS, options.ciphers.c_str());
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
    curl_easy_setopt(curl_->handle, CURLOPT_LOCALPORT, local_port);
}

void Session::SetLocalPortRange(const LocalPortRange& local_port_range) {
    curl_easy_setopt(curl_->handle, CURLOPT_LOCALPORTRANGE, local_port_range);
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

        default: // Should not happen
            throw std::invalid_argument("Invalid/Unknown HTTP version type.");
            break;
    }
}

void Session::SetRange(const Range& range) {
    std::string range_str = range.str();
    curl_easy_setopt(curl_->handle, CURLOPT_RANGE, range_str.c_str());
}

void Session::SetMultiRange(const MultiRange& multi_range) {
    std::string multi_range_str = multi_range.str();
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
    cpr_off_t downloadFileLenth = -1;
    curl_easy_setopt(curl_->handle, CURLOPT_URL, url_.c_str());

    std::string protocol = url_.str().substr(0, url_.str().find(':'));
    if (proxies_.has(protocol)) {
        curl_easy_setopt(curl_->handle, CURLOPT_PROXY, proxies_[protocol].c_str());
        if (proxyAuth_.has(protocol)) {
            curl_easy_setopt(curl_->handle, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
            curl_easy_setopt(curl_->handle, CURLOPT_PROXYUSERPWD, proxyAuth_[protocol]);
        }
    }

    curl_easy_setopt(curl_->handle, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 1);
    if (curl_easy_perform(curl_->handle) == CURLE_OK) {
        curl_easy_getinfo(curl_->handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &downloadFileLenth);
    }
    return downloadFileLenth;
}

void Session::ResponseStringReserve(size_t size) {
    response_string_reserve_size_ = size;
}

Response Session::Delete() {
    PrepareDelete();
    return makeRequest();
}

Response Session::Download(const WriteCallback& write) {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, nullptr);

    SetWriteCallback(write);

    return makeDownloadRequest();
}

Response Session::Download(std::ofstream& file) {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_->handle, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl_->handle, CURLOPT_WRITEFUNCTION, cpr::util::writeFileFunction);
    curl_easy_setopt(curl_->handle, CURLOPT_WRITEDATA, &file);
    curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, nullptr);

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
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this]() { return shared_this->Delete(); });
}

AsyncResponse Session::DownloadAsync(const WriteCallback& write) {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this, write]() { return shared_this->Download(write); });
}

AsyncResponse Session::DownloadAsync(std::ofstream& file) {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this, &file]() { return shared_this->Download(file); });
}

AsyncResponse Session::HeadAsync() {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this]() { return shared_this->Head(); });
}

AsyncResponse Session::OptionsAsync() {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this]() { return shared_this->Options(); });
}

AsyncResponse Session::PatchAsync() {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this]() { return shared_this->Patch(); });
}

AsyncResponse Session::PostAsync() {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this]() { return shared_this->Post(); });
}

AsyncResponse Session::PutAsync() {
    auto shared_this = GetSharedPtrFromThis();
    return async([shared_this]() { return shared_this->Put(); });
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
    if (hasBodyOrPayload_) {
        curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, nullptr);
    } else {
        curl_easy_setopt(curl_->handle, CURLOPT_POSTFIELDS, readcb_.callback ? nullptr : "");
        curl_easy_setopt(curl_->handle, CURLOPT_CUSTOMREQUEST, "POST");
    }
    prepareCommon();
}

void Session::PreparePut() {
    curl_easy_setopt(curl_->handle, CURLOPT_NOBODY, 0L);
    if (!hasBodyOrPayload_ && readcb_.callback) {
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

Response Session::Complete(CURLcode curl_error) {
    curl_slist* raw_cookies{nullptr};
    curl_easy_getinfo(curl_->handle, CURLINFO_COOKIELIST, &raw_cookies);
    Cookies cookies = util::parseCookies(raw_cookies);
    curl_slist_free_all(raw_cookies);

    // Reset the has no body property:
    hasBodyOrPayload_ = false;

    std::string errorMsg = curl_->error.data();
    return Response(curl_, std::move(response_string_), std::move(header_string_), std::move(cookies), Error(curl_error, std::move(errorMsg)));
}

void Session::AddInterceptor(const std::shared_ptr<Interceptor>& pinterceptor) {
    interceptors_.push(pinterceptor);
}

Response Session::proceed() {
    prepareCommon();
    return makeRequest();
}

// clang-format off
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
void Session::SetOption(AcceptEncoding&& accept_encoding) { SetAcceptEncoding(accept_encoding); }
// clang-format on
} // namespace cpr
