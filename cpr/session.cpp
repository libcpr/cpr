#include "cpr/session.h"

#include <algorithm>
#include <functional>
#include <string>

#include <curl/curl.h>

#include "cpr/curlholder.h"
#include "cpr/util.h"

namespace cpr {

class Session::Impl {
  public:
    Impl();

    void SetUrl(const Url& url);
    void SetParameters(const Parameters& parameters);
    void SetParameters(Parameters&& parameters);
    void SetHeader(const Header& header);
    void SetTimeout(const Timeout& timeout);
    void SetAuth(const Authentication& auth);
    void SetDigest(const Digest& auth);
    void SetPayload(Payload&& payload);
    void SetPayload(const Payload& payload);
    void SetProxies(Proxies&& proxies);
    void SetProxies(const Proxies& proxies);
    void SetMultipart(Multipart&& multipart);
    void SetMultipart(const Multipart& multipart);
    void SetRedirect(const bool& redirect);
    void SetMaxRedirects(const long& max_redirects);
    void SetCookies(const Cookies& cookies);
    void SetBody(Body&& body);
    void SetBody(const Body& body);

    Response Delete();
    Response Get();
    Response Head();
    Response Options();
    Response Patch();
    Response Post();
    Response Put();

  private:
    std::unique_ptr<CurlHolder, std::function<void(CurlHolder*)>> curl_;
    Url url_;
    Parameters parameters_;
    Proxies proxies_;

    Response makeRequest(CURL* curl);
    static void freeHolder(CurlHolder* holder);
    static CurlHolder* newHolder();
};

Session::Impl::Impl() {
    curl_ = std::unique_ptr<CurlHolder, std::function<void(CurlHolder*) >>(newHolder(),
                                                                           &Impl::freeHolder);
    auto curl = curl_->handle;
    if (curl) {
        // Set up some sensible defaults
        auto version_info = curl_version_info(CURLVERSION_NOW);
        auto version = std::string{"curl/"} + std::string{version_info->version};
        curl_easy_setopt(curl, CURLOPT_USERAGENT, version.data());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_->error);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
#ifdef INSECURE_CURL
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
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

void Session::Impl::freeHolder(CurlHolder* holder) {
    curl_easy_cleanup(holder->handle);
    curl_slist_free_all(holder->chunk);
    delete holder;
}

CurlHolder* Session::Impl::newHolder() {
    CurlHolder* holder = new CurlHolder();
    holder->handle = curl_easy_init();
    return holder;
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
    auto curl = curl_->handle;
    if (curl) {
        struct curl_slist* chunk = NULL;
        for (auto item = header.cbegin(); item != header.cend(); ++item) {
            auto header_string = std::string{item->first};
            if (item->second.empty()) {
                header_string += ";";
            } else {
                header_string += ": " + item->second;
            }
            chunk = curl_slist_append(chunk, header_string.data());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_->chunk = chunk;
        }
    }
}

void Session::Impl::SetTimeout(const Timeout& timeout) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout.ms);
    }
}

void Session::Impl::SetAuth(const Authentication& auth) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.GetAuthString());
    }
}

void Session::Impl::SetDigest(const Digest& auth) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.GetAuthString());
    }
}

void Session::Impl::SetPayload(Payload&& payload) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.content.length());
        curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, payload.content.data());
    }
}

void Session::Impl::SetPayload(const Payload& payload) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.content.length());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.content.data());
    }
}

void Session::Impl::SetProxies(const Proxies& proxies) {
    proxies_ = proxies;
}

void Session::Impl::SetProxies(Proxies&& proxies) {
    proxies_ = std::move(proxies);
}

void Session::Impl::SetMultipart(Multipart&& multipart) {
    auto curl = curl_->handle;
    if (curl) {
        struct curl_httppost* formpost = NULL;
        struct curl_httppost* lastptr = NULL;

        for (auto& part : multipart.parts) {
            auto content_option = CURLFORM_COPYCONTENTS;
            if (part.is_file) {
                content_option = CURLFORM_FILE;
            }
            if (part.content_type.empty()) {
                curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, part.name.data(),
                             content_option, part.value.data(), CURLFORM_END);
            } else {
                curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, part.name.data(),
                             content_option, part.value.data(), CURLFORM_CONTENTTYPE,
                             part.content_type.data(), CURLFORM_END);
            }
        }
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    }
}

void Session::Impl::SetMultipart(const Multipart& multipart) {
    auto curl = curl_->handle;
    if (curl) {
        struct curl_httppost* formpost = NULL;
        struct curl_httppost* lastptr = NULL;

        for (auto& part : multipart.parts) {
            auto content_option = CURLFORM_PTRCONTENTS;
            if (part.is_file) {
                content_option = CURLFORM_FILE;
            }
            if (part.content_type.empty()) {
                curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, part.name.data(),
                             content_option, part.value.data(), CURLFORM_END);
            } else {
                curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, part.name.data(),
                             content_option, part.value.data(), CURLFORM_CONTENTTYPE,
                             part.content_type.data(), CURLFORM_END);
            }
        }
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    }
}

void Session::Impl::SetRedirect(const bool& redirect) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, long(redirect));
    }
}

void Session::Impl::SetMaxRedirects(const long& max_redirects) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, max_redirects);
    }
}

void Session::Impl::SetCookies(const Cookies& cookies) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
        curl_easy_setopt(curl, CURLOPT_COOKIE, cookies.GetEncoded().data());
    }
}

void Session::Impl::SetBody(Body&& body) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
        curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, body.data());
    }
}

void Session::Impl::SetBody(const Body& body) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.data());
    }
}

Response Session::Impl::Delete() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 0L);
        curl_easy_setopt(curl, CURLOPT_POST, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }

    return makeRequest(curl);
}

Response Session::Impl::Get() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_POST, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    }

    return makeRequest(curl);
}

Response Session::Impl::Head() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 0L);
        curl_easy_setopt(curl, CURLOPT_POST, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    }

    return makeRequest(curl);
}

Response Session::Impl::Options() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 0L);
        curl_easy_setopt(curl, CURLOPT_POST, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
    }

    return makeRequest(curl);
}

Response Session::Impl::Patch() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    }

    return makeRequest(curl);
}

Response Session::Impl::Post() {
    return makeRequest(curl_->handle);
}

Response Session::Impl::Put() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    }

    return makeRequest(curl);
}

Response Session::Impl::makeRequest(CURL* curl) {
    if (!parameters_.content.empty()) {
        Url new_url{url_ + "?" + parameters_.content};
        curl_easy_setopt(curl, CURLOPT_URL, new_url.data());
    } else {
        curl_easy_setopt(curl, CURLOPT_URL, url_.data());
    }

    auto protocol = url_.substr(0, url_.find(':'));
    if (proxies_.has(protocol)) {
        curl_easy_setopt(curl, CURLOPT_PROXY, proxies_[protocol].data());
    } else {
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
    }

    curl_->error[0] = '\0';

    std::string response_string;
    std::string header_string;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cpr::util::writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

    auto curl_error = curl_easy_perform(curl);

    char* raw_url;
    long response_code;
    double elapsed;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &raw_url);

    Error error(curl_error, curl_->error);

    Cookies cookies;
    struct curl_slist* raw_cookies;
    curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &raw_cookies);
    for (struct curl_slist* nc = raw_cookies; nc; nc = nc->next) {
        auto tokens = cpr::util::split(nc->data, '\t');
        auto value = tokens.back();
        tokens.pop_back();
        cookies[tokens.back()] = value;
    }
    curl_slist_free_all(raw_cookies);

    auto header = cpr::util::parseHeader(header_string);
    response_string = cpr::util::parseResponse(response_string);
    return Response{response_code, response_string, header, raw_url, elapsed, cookies, error};
}

// clang-format off
Session::Session() : pimpl_{ new Impl{} } {}
Session::~Session() {}
void Session::SetUrl(const Url& url) { pimpl_->SetUrl(url); }
void Session::SetParameters(const Parameters& parameters) { pimpl_->SetParameters(parameters); }
void Session::SetParameters(Parameters&& parameters) { pimpl_->SetParameters(std::move(parameters)); }
void Session::SetHeader(const Header& header) { pimpl_->SetHeader(header); }
void Session::SetTimeout(const Timeout& timeout) { pimpl_->SetTimeout(timeout); }
void Session::SetAuth(const Authentication& auth) { pimpl_->SetAuth(auth); }
void Session::SetDigest(const Digest& auth) { pimpl_->SetDigest(auth); }
void Session::SetPayload(const Payload& payload) { pimpl_->SetPayload(payload); }
void Session::SetPayload(Payload&& payload) { pimpl_->SetPayload(std::move(payload)); }
void Session::SetProxies(const Proxies& proxies) { pimpl_->SetProxies(proxies); }
void Session::SetProxies(Proxies&& proxies) { pimpl_->SetProxies(std::move(proxies)); }
void Session::SetMultipart(const Multipart& multipart) { pimpl_->SetMultipart(multipart); }
void Session::SetMultipart(Multipart&& multipart) { pimpl_->SetMultipart(std::move(multipart)); }
void Session::SetRedirect(const bool& redirect) { pimpl_->SetRedirect(redirect); }
void Session::SetMaxRedirects(const long& max_redirects) { pimpl_->SetMaxRedirects(max_redirects); }
void Session::SetCookies(const Cookies& cookies) { pimpl_->SetCookies(cookies); }
void Session::SetBody(const Body& body) { pimpl_->SetBody(body); }
void Session::SetBody(Body&& body) { pimpl_->SetBody(std::move(body)); }
void Session::SetOption(const Url& url) { pimpl_->SetUrl(url); }
void Session::SetOption(const Parameters& parameters) { pimpl_->SetParameters(parameters); }
void Session::SetOption(Parameters&& parameters) { pimpl_->SetParameters(std::move(parameters)); }
void Session::SetOption(const Header& header) { pimpl_->SetHeader(header); }
void Session::SetOption(const Timeout& timeout) { pimpl_->SetTimeout(timeout); }
void Session::SetOption(const Authentication& auth) { pimpl_->SetAuth(auth); }
void Session::SetOption(const Digest& auth) { pimpl_->SetDigest(auth); }
void Session::SetOption(const Payload& payload) { pimpl_->SetPayload(payload); }
void Session::SetOption(Payload&& payload) { pimpl_->SetPayload(std::move(payload)); }
void Session::SetOption(const Proxies& proxies) { pimpl_->SetProxies(proxies); }
void Session::SetOption(Proxies&& proxies) { pimpl_->SetProxies(std::move(proxies)); }
void Session::SetOption(const Multipart& multipart) { pimpl_->SetMultipart(multipart); }
void Session::SetOption(Multipart&& multipart) { pimpl_->SetMultipart(std::move(multipart)); }
void Session::SetOption(const bool& redirect) { pimpl_->SetRedirect(redirect); }
void Session::SetOption(const long& max_redirects) { pimpl_->SetMaxRedirects(max_redirects); }
void Session::SetOption(const Cookies& cookies) { pimpl_->SetCookies(cookies); }
void Session::SetOption(const Body& body) { pimpl_->SetBody(body); }
void Session::SetOption(Body&& body) { pimpl_->SetBody(std::move(body)); }
Response Session::Delete() { return pimpl_->Delete(); }
Response Session::Get() { return pimpl_->Get(); }
Response Session::Head() { return pimpl_->Head(); }
Response Session::Options() { return pimpl_->Options(); }
Response Session::Patch() { return pimpl_->Patch(); }
Response Session::Post() { return pimpl_->Post(); }
Response Session::Put() { return pimpl_->Put(); }
// clang-format on

} // namespace cpr
