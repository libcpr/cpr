#include "session.h"

#include <functional>

#include <curl/curl.h>

#include "curlholder.h"
#include "payload.h"
#include "util.h"


class Session::Impl {
  public:
    Impl();

    void SetUrl(const Url& url);
    void SetUrl(const Url& url, const Parameters& parameters);
    void SetHeader(const Header& header);
    void SetTimeout(const Timeout& timeout);
    void SetAuth(const Authentication& auth);
    void SetPayload(const Payload& payload);
    void SetMultipart(const Multipart& multipart);
    void SetRedirect(const bool& redirect);
    void SetMaxRedirects(const long& max_redirects);
    // void SetCookie(); Unimplemented
    // void SetCookies(); Unimplemented

    Response Get();
    Response Post();

  private:
    std::unique_ptr<CurlHolder, std::function<void(CurlHolder*)>> curl_;

    Response makeRequest(CURL* curl);
    static void freeHolder(CurlHolder* holder);
    static CurlHolder* newHolder();
};

Session::Impl::Impl() {
    curl_ = std::unique_ptr<CurlHolder, std::function<void(CurlHolder*)>>(newHolder(), &Impl::freeHolder);
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
}

CurlHolder* Session::Impl::newHolder() {
    CurlHolder* holder = new CurlHolder();
    holder->handle = curl_easy_init();
    return holder;
}

void Session::Impl::SetUrl(const Url& url) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.data());
    }
}

void Session::Impl::SetUrl(const Url& url, const Parameters& parameters) {
    auto curl = curl_->handle;
    if (curl) {
        Url new_url{url + "?"};
        for(auto parameter = parameters.cbegin(); parameter != parameters.cend(); ++parameter) {
            new_url += parameter->first + "=" + parameter->second + "&";
        }
        new_url = new_url.substr(0, new_url.size() - 1);
        curl_easy_setopt(curl, CURLOPT_URL, new_url.data());
    }
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
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, timeout);
    }
}

void Session::Impl::SetAuth(const Authentication& auth) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.GetAuthString());
    }
}

void Session::Impl::SetPayload(const Payload& payload) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.content.length());
        curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, payload.content.data());
    }
}

void Session::Impl::SetMultipart(const Multipart& multipart) {
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
                curl_formadd(&formpost,
                             &lastptr,
                             CURLFORM_COPYNAME, part.name.data(),
                             content_option, part.value.data(),
                             CURLFORM_END);
            } else {
                std::cout << " Adding content type " << part.content_type.data() << std::endl;
                curl_formadd(&formpost,
                             &lastptr,
                             CURLFORM_COPYNAME, part.name.data(),
                             content_option, part.value.data(),
                             CURLFORM_CONTENTTYPE, part.content_type.data(),
                             CURLFORM_END);
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

Response Session::Impl::Get() {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_POST, 0L);
    }

    return makeRequest(curl);
}

Response Session::Impl::Post() {
    return makeRequest(curl_->handle);
}

Response Session::Impl::makeRequest(CURL* curl) {
    std::string response_string;
    std::string header_string;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cpr::util::writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

    curl_easy_perform(curl);
    
    char* raw_url;
    long response_code;
    double elapsed;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &raw_url);
    auto header = cpr::util::parseHeader(header_string);
    response_string = cpr::util::parseResponse(response_string);
    return Response{response_code, response_string, header, raw_url, elapsed};
}

Session::Session() : pimpl_{ new Impl{} } {}
Session::~Session() {}
void Session::SetUrl(const Url& url) { pimpl_->SetUrl(url); }
void Session::SetUrl(const Url& url, const Parameters& parameters) { pimpl_->SetUrl(url, parameters); }
void Session::SetHeader(const Header& header) { pimpl_->SetHeader(header); }
void Session::SetTimeout(const Timeout& timeout) { pimpl_->SetTimeout(timeout); }
void Session::SetAuth(const Authentication& auth) { pimpl_->SetAuth(auth); }
void Session::SetPayload(const Payload& payload) { pimpl_->SetPayload(payload); }
void Session::SetMultipart(const Multipart& multipart) { pimpl_->SetMultipart(multipart); }
void Session::SetRedirect(const bool& redirect) { pimpl_->SetRedirect(redirect); }
void Session::SetMaxRedirects(const long& max_redirects) { pimpl_->SetMaxRedirects(max_redirects); }
// void SetCookie(); Unimplemented
// void SetCookies(); Unimplemented
Response Session::Get() { return pimpl_->Get(); }
Response Session::Post() { return pimpl_->Post(); }
