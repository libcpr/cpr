#include "cpr/multiperform.h"

#include "cpr/callback.h"
#include "cpr/curlmultiholder.h"
#include "cpr/interceptor.h"
#include "cpr/response.h"
#include "cpr/session.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <curl/curl.h>
#include <curl/curlver.h>
#include <curl/multi.h>
#include <functional>
#include <iosfwd>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace cpr {

MultiPerform::MultiPerform() : multicurl_(new CurlMultiHolder()) {
    current_interceptor_ = interceptors_.end();
    first_interceptor_ = interceptors_.end();
}

MultiPerform::MultiPerform(MultiPerform&& old) noexcept {
    *this = std::move(old);
}

MultiPerform& MultiPerform::operator=(MultiPerform&& old) noexcept {
    sessions_ = std::move(old.sessions_);
    multicurl_ = std::move(old.multicurl_);
    interceptors_ = std::move(old.interceptors_);
    current_interceptor_ = interceptors_.end();
    first_interceptor_ = interceptors_.end();
    return *this;
}

MultiPerform::~MultiPerform() {
    // Unlock all sessions
    for (const auto& [session, method] : sessions_) {
        session->isUsedInMultiPerform = false;

        // Remove easy handle from multi handle
        const CURLMcode error_code = curl_multi_remove_handle(multicurl_->handle, session->curl_->handle);
        if (error_code) {
            std::cerr << "curl_multi_remove_handle() failed, code " << static_cast<int>(error_code) << '\n';
        }
    }
}

void MultiPerform::AddSession(std::shared_ptr<Session>& session, HttpMethod method) {
    // Check if this multiperform is download only
    if (((method != HttpMethod::DOWNLOAD_REQUEST && is_download_multi_perform) && method != HttpMethod::UNDEFINED) || (method == HttpMethod::DOWNLOAD_REQUEST && !is_download_multi_perform && !sessions_.empty())) {
        // Currently it is not possible to mix download and non-download methods, as download needs additional parameters
        throw std::invalid_argument("Failed to add session: Cannot mix download and non-download methods!");
    }

    // Set download only if neccessary
    if (method == HttpMethod::DOWNLOAD_REQUEST) {
        is_download_multi_perform = true;
    }

    // Lock session to the multihandle
    session->isUsedInMultiPerform = true;

    // Add session to sessions_
    sessions_.emplace_back(session, method);
}

void MultiPerform::RemoveSession(const std::shared_ptr<Session>& session) {
    if (sessions_.empty()) {
        throw std::invalid_argument("Failed to find session!");
    }

    // Unlock session
    session->isUsedInMultiPerform = false;

    // Remove session from sessions_
    auto it = std::find_if(sessions_.begin(), sessions_.end(), [&session](const std::pair<std::shared_ptr<Session>, HttpMethod>& pair) { return session->curl_->handle == pair.first->curl_->handle; });
    if (it == sessions_.end()) {
        throw std::invalid_argument("Failed to find session!");
    }
    sessions_.erase(it);

    // Reset download only if empty
    if (sessions_.empty()) {
        is_download_multi_perform = false;
    }
}

std::vector<std::pair<std::shared_ptr<Session>, MultiPerform::HttpMethod>>& MultiPerform::GetSessions() {
    return sessions_;
}

const std::vector<std::pair<std::shared_ptr<Session>, MultiPerform::HttpMethod>>& MultiPerform::GetSessions() const {
    return sessions_;
}

void MultiPerform::DoMultiPerform() {
    // Do multi perform until every handle has finished
    int still_running{0};
    for (const auto& [session, _] : sessions_) {
        const CURLMcode error_code = curl_multi_add_handle(multicurl_->handle, session->curl_->handle);
        if (error_code && error_code != CURLM_ADDED_ALREADY) {
            std::cerr << "curl_multi_add_handle() failed, code " << static_cast<int>(error_code) << '\n';
        }
    }
    do {
        CURLMcode error_code = curl_multi_perform(multicurl_->handle, &still_running);
        if (error_code) {
            std::cerr << "curl_multi_perform() failed, code " << static_cast<int>(error_code) << '\n';
            break;
        }

        if (still_running) {
            const int timeout_ms{250};
#if LIBCURL_VERSION_NUM >= 0x074200 // 7.66.0
            error_code = curl_multi_poll(multicurl_->handle, nullptr, 0, timeout_ms, nullptr);
            if (error_code) {
                std::cerr << "curl_multi_poll() failed, code " << static_cast<int>(error_code) << '\n';
#else
            error_code = curl_multi_wait(multicurl_->handle, nullptr, 0, timeout_ms, nullptr);
            if (error_code) {
                std::cerr << "curl_multi_wait() failed, code " << static_cast<int>(error_code) << '\n';

#endif
                break;
            }
        }
    } while (still_running);
}

std::vector<Response> MultiPerform::ReadMultiInfo(const std::function<Response(Session&, CURLcode)>& complete_function) {
    // Get infos and create Response objects
    std::vector<Response> responses;
    struct CURLMsg* info{nullptr};
    do {
        int msgq = 0;

        // Read info from multihandle
        info = curl_multi_info_read(multicurl_->handle, &msgq);

        if (info) {
            // Find current session
            auto it = std::find_if(sessions_.begin(), sessions_.end(), [&info](const std::pair<std::shared_ptr<Session>, HttpMethod>& pair) { return pair.first->curl_->handle == info->easy_handle; });
            if (it == sessions_.end()) {
                std::cerr << "Failed to find current session!" << '\n';
                break;
            }
            const std::shared_ptr<Session> current_session = (*it).first;

            // Add response object
            // NOLINTNEXTLINE (cppcoreguidelines-pro-type-union-access)
            responses.push_back(complete_function(*current_session, info->data.result));
        }
    } while (info);

    for (const auto& [session, _] : sessions_) {
        const CURLMcode error_code = curl_multi_remove_handle(multicurl_->handle, session->curl_->handle);
        if (error_code) {
            std::cerr << "curl_multi_remove_handle() failed, code " << static_cast<int>(error_code) << '\n';
        }
    }

    // Sort response objects to match order of added sessions
    std::vector<Response> sorted_responses;
    for (const auto& [session, _] : sessions_) {
        Session& current_session = *session;
        auto it = std::find_if(responses.begin(), responses.end(), [&current_session](const Response& response) { return current_session.curl_->handle == response.curl_->handle; });
        const Response current_response = *it; // NOLINT (performance-unnecessary-copy-initialization) False positive
        // Erase response from original vector to increase future search speed
        responses.erase(it);
        sorted_responses.push_back(current_response);
    }
    return sorted_responses;
}

std::vector<Response> MultiPerform::MakeRequest() {
    const std::optional<std::vector<Response>> r = intercept();
    if (r.has_value()) {
        return r.value();
    }

    DoMultiPerform();
    return ReadMultiInfo([](Session& session, CURLcode curl_error) -> Response { return session.Complete(curl_error); });
}

std::vector<Response> MultiPerform::MakeDownloadRequest() {
    const std::optional<std::vector<Response>> r = intercept();
    if (r.has_value()) {
        return r.value();
    }

    DoMultiPerform();
    return ReadMultiInfo([](Session& session, CURLcode curl_error) -> Response { return session.CompleteDownload(curl_error); });
}

void MultiPerform::PrepareSessions() {
    for (const auto& [session, method] : sessions_) {
        switch (method) {
            case HttpMethod::GET_REQUEST:
                session->PrepareGet();
                break;
            case HttpMethod::POST_REQUEST:
                session->PreparePost();
                break;
            case HttpMethod::PUT_REQUEST:
                session->PreparePut();
                break;
            case HttpMethod::DELETE_REQUEST:
                session->PrepareDelete();
                break;
            case HttpMethod::PATCH_REQUEST:
                session->PreparePatch();
                break;
            case HttpMethod::HEAD_REQUEST:
                session->PrepareHead();
                break;
            case HttpMethod::OPTIONS_REQUEST:
                session->PrepareOptions();
                break;
            default:
                std::cerr << "PrepareSessions failed: Undefined HttpMethod or download without arguments!" << '\n';
                return;
        }
    }
}

void MultiPerform::PrepareDownloadSession(size_t sessions_index, const WriteCallback& write) {
    const auto& [session, method] = sessions_[sessions_index];
    switch (method) {
        case HttpMethod::DOWNLOAD_REQUEST:
            session->PrepareDownload(write);
            break;
        default:
            std::cerr << "PrepareSessions failed: Undefined HttpMethod or non download method with arguments!" << '\n';
            return;
    }
}

void MultiPerform::PrepareDownloadSession(size_t sessions_index, std::ofstream& file) {
    const auto& [session, method] = sessions_[sessions_index];
    switch (method) {
        case HttpMethod::DOWNLOAD_REQUEST:
            session->PrepareDownload(file);
            break;
        default:
            std::cerr << "PrepareSessions failed: Undefined HttpMethod or non download method with arguments!" << '\n';
            return;
    }
}

void MultiPerform::SetHttpMethod(HttpMethod method) {
    for (auto& [_, session_method] : sessions_) {
        session_method = method;
    }
}

void MultiPerform::PrepareGet() {
    SetHttpMethod(HttpMethod::GET_REQUEST);
    PrepareSessions();
}

void MultiPerform::PrepareDelete() {
    SetHttpMethod(HttpMethod::DELETE_REQUEST);
    PrepareSessions();
}

void MultiPerform::PreparePut() {
    SetHttpMethod(HttpMethod::PUT_REQUEST);
    PrepareSessions();
}

void MultiPerform::PreparePatch() {
    SetHttpMethod(HttpMethod::PATCH_REQUEST);
    PrepareSessions();
}

void MultiPerform::PrepareHead() {
    SetHttpMethod(HttpMethod::HEAD_REQUEST);
    PrepareSessions();
}

void MultiPerform::PrepareOptions() {
    SetHttpMethod(HttpMethod::OPTIONS_REQUEST);
    PrepareSessions();
}

void MultiPerform::PreparePost() {
    SetHttpMethod(HttpMethod::POST_REQUEST);
    PrepareSessions();
}

std::vector<Response> MultiPerform::Get() {
    PrepareGet();
    return MakeRequest();
}

std::vector<Response> MultiPerform::Delete() {
    PrepareDelete();
    return MakeRequest();
}

std::vector<Response> MultiPerform::Put() {
    PreparePut();
    return MakeRequest();
}

std::vector<Response> MultiPerform::Head() {
    PrepareHead();
    return MakeRequest();
}

std::vector<Response> MultiPerform::Options() {
    PrepareOptions();
    return MakeRequest();
}

std::vector<Response> MultiPerform::Patch() {
    PreparePatch();
    return MakeRequest();
}

std::vector<Response> MultiPerform::Post() {
    PreparePost();
    return MakeRequest();
}

std::vector<Response> MultiPerform::Perform() {
    PrepareSessions();
    return MakeRequest();
}

std::vector<Response> MultiPerform::proceed() {
    // Check if this multiperform mixes download and non download requests
    if (!sessions_.empty()) {
        const bool new_is_download_multi_perform = sessions_.front().second == HttpMethod::DOWNLOAD_REQUEST;

        for (const auto& [_, method] : sessions_) {
            if ((new_is_download_multi_perform && method != HttpMethod::DOWNLOAD_REQUEST) || (!new_is_download_multi_perform && method == HttpMethod::DOWNLOAD_REQUEST)) {
                throw std::invalid_argument("Failed to proceed with session: Cannot mix download and non-download methods!");
            }
        }
        is_download_multi_perform = new_is_download_multi_perform;
    }

    PrepareSessions();
    return MakeRequest();
}

const std::optional<std::vector<Response>> MultiPerform::intercept() {
    if (current_interceptor_ == interceptors_.end()) {
        current_interceptor_ = first_interceptor_;
    } else {
        current_interceptor_++;
    }

    if (current_interceptor_ != interceptors_.end()) {
        auto icpt = current_interceptor_;
        // Nested makeRequest() start at first_interceptor_, thus excluding previous interceptors.
        first_interceptor_ = current_interceptor_;
        ++first_interceptor_;

        const std::optional<std::vector<Response>> r = (*current_interceptor_)->intercept(*this);

        first_interceptor_ = icpt;

        return r;
    }
    return std::nullopt;
}

void MultiPerform::AddInterceptor(const std::shared_ptr<InterceptorMulti>& pinterceptor) {
    // Shall only add before first interceptor run
    assert(current_interceptor_ == interceptors_.end());
    interceptors_.push_back(pinterceptor);
    first_interceptor_ = interceptors_.begin();
}

} // namespace cpr
