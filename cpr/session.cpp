#include "session.h"

#include <functional>

#include <curl/curl.h>

#include "curlholder.h"
#include "util.h"


class Session::Impl {
  public:
    Impl();

    void SetUrl(Url url);
    void SetUrl(Url url, Parameters parameters);
    void SetHeader(Header header);
    void SetTimeout(Timeout timeout);
    void SetAuth(Authentication auth);
    void SetPayload(Payload payload);
    void SetRedirect(bool redirect);
    // void SetCookie(); Unimplemented
    // void SetCookies(); Unimplemented

    Response Get();
    Response Post();

  private:
    std::unique_ptr<CurlHolder, std::function<void(CurlHolder*)>> curl_;

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

void Session::Impl::SetUrl(Url url) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.data());
    }
}

void Session::Impl::SetUrl(Url url, Parameters parameters) {
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

void Session::Impl::SetHeader(Header header) {
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

void Session::Impl::SetTimeout(long timeout) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, timeout);
    }
}

void Session::Impl::SetAuth(Authentication auth) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.GetAuthString());
    }
}

void Session::Impl::SetPayload(Payload payload) {
    auto curl = curl_->handle;
    if (curl) {
        struct curl_slist* chunk = NULL;
        auto payload_string = std::string{};
        for (auto item = payload.cbegin(); item != payload.cend(); ++item) {
            if (!payload_string.empty()) {
                payload_string += "&";
            }
            payload_string += item->first + "=" + item->second;
        }
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_string.data());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload_string.length());
    }
}

void Session::Impl::SetRedirect(bool redirect) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, long(redirect));
    }
}

Response Session::Impl::Get() {
    auto curl = curl_->handle;
    CURLcode res;
    Response response{0, "Curl could not start", Header{}, Url{}, 0.0};

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_POST, 0L);
        std::string response_string;
        std::string header_string;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cpr::util::writeFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

        res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }
        
        char* raw_url;
        long response_code;
        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &raw_url);
        auto header = cpr::util::parseHeader(header_string);
        response_string = cpr::util::parseResponse(response_string);
        response = {response_code, response_string, header, raw_url, elapsed};
    }

    return response;
}

Response Session::Impl::Post() {
    auto curl = curl_->handle;
    CURLcode res;
    Response response{0, "Curl could not start", Header{}, Url{}, 0.0};

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 0L);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        std::string response_string;
        std::string header_string;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cpr::util::writeFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

        res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }
        
        char* raw_url;
        long response_code;
        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &raw_url);
        auto header = cpr::util::parseHeader(header_string);
        response_string = cpr::util::parseResponse(response_string);
        response = {response_code, response_string, header, raw_url, elapsed};
    }

    return response;
}

Session::Session() : pimpl_{ new Impl{} } {}
Session::~Session() {}
void Session::SetUrl(Url url) { pimpl_->SetUrl(url); }
void Session::SetUrl(Url url, Parameters parameters) { pimpl_->SetUrl(url, parameters); }
void Session::SetHeader(Header header) { pimpl_->SetHeader(header); }
void Session::SetTimeout(Timeout timeout) { pimpl_->SetTimeout(timeout); }
void Session::SetAuth(Authentication auth) { pimpl_->SetAuth(auth); }
void Session::SetPayload(Payload payload) { pimpl_->SetPayload(payload); }
void Session::SetRedirect(bool redirect) { pimpl_->SetRedirect(redirect); }
// void SetCookie(); Unimplemented
// void SetCookies(); Unimplemented
Response Session::Get() { return pimpl_->Get(); }
Response Session::Post() { return pimpl_->Post(); }
