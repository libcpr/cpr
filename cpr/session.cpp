#include "session.h"

#include "util.h"


Session::Session() {
    curl_ = {cpr::util::newHolder(), &cpr::util::freeHolder};
    auto curl = curl_->handle;
    if (curl) {
        // Set up some sensible defaults
        auto version_info = curl_version_info(CURLVERSION_NOW);
        auto version = std::string{"curl/"} + std::string{version_info->version};
        curl_easy_setopt(curl, CURLOPT_USERAGENT, version.data());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }
}

void Session::SetUrl(Url url) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.data());
    }
}

void Session::SetUrl(Url url, Parameters parameters) {
    auto curl = curl_->handle;
    if (curl) {
        Url new_url{url + "?"};
        for(auto parameter = parameters.cbegin(); parameter != parameters.cend(); ++parameter) {
            new_url += parameter->first + "=" + parameter->second;
        }
        curl_easy_setopt(curl, CURLOPT_URL, new_url.data());
    }
}

void Session::SetHeader(Header header) {
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

void Session::SetTimeout(long timeout) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, timeout);
    }
}

void Session::SetAuth(Authentication auth) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_USERPWD, auth.GetAuthString());
    }
}

void Session::SetPayload(Payload payload) {
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

void Session::SetRedirect(bool redirect) {
    auto curl = curl_->handle;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, long(redirect));
    }
}

Response Session::Get() {
    auto curl = curl_->handle;
    CURLcode res;

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
        Response response{response_code, response_string, header, raw_url, elapsed};
        return response;
    }

    return Response{0, "Curl could not start", Header{}, Url{}, 0.0};
}

Response Session::Post() {
    auto curl = curl_->handle;
    CURLcode res;

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
        Response response{response_code, response_string, header, raw_url, elapsed};
        return response;
    }

    return Response{0, "Curl could not start", Header{}, Url{}, 0.0};
}
