#include "cpr/multiperform.h"

#include <algorithm>
#include <iostream>

namespace cpr {

MultiPerform::MultiPerform() : multicurl_(new CurlMultiHolder()) {}

MultiPerform::~MultiPerform() {
    // Unock all sessions
    for (std::pair<std::shared_ptr<Session>, HttpMethod>& pair : sessions_) {
        pair.first->isUsedInMultiPerform = false;
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

    // Add easy handle to multi handle
    CURLMcode error_code = curl_multi_add_handle(multicurl_->handle, session->curl_->handle);
    if (error_code) {
        std::cerr << "curl_multi_add_handle() failed, code " << static_cast<int>(error_code) << std::endl;
        return;
    }

    // Lock session to the multihandle
    session->isUsedInMultiPerform = true;

    // Add session to sessions_
    sessions_.emplace_back(session, method);
}

void MultiPerform::RemoveSession(const std::shared_ptr<Session>& session) {
    // Remove easy handle from multihandle
    CURLMcode error_code = curl_multi_remove_handle(multicurl_->handle, session->curl_->handle);
    if (error_code) {
        std::cerr << "curl_multi_remove_handle() failed, code " << static_cast<int>(error_code) << std::endl;
        return;
    }

    // Unock session
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

void MultiPerform::DoMultiPerform() {
    // Do multi perform until every handle has finished
    int still_running{0};
    do {
        CURLMcode error_code = curl_multi_perform(multicurl_->handle, &still_running);
        if (error_code) {
            std::cerr << "curl_multi_perform() failed, code " << static_cast<int>(error_code) << std::endl;
            break;
        }

        if (still_running) {
            const int timeout_ms{250};
            error_code = curl_multi_poll(multicurl_->handle, nullptr, 0, timeout_ms, nullptr);
            if (error_code) {
                std::cerr << "curl_multi_poll() failed, code " << static_cast<int>(error_code) << std::endl;
                break;
            }
        }
    } while (still_running);
}

std::vector<Response> MultiPerform::ReadMultiInfo(std::function<Response(Session&, CURLcode)>&& complete_function) {
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
                std::cerr << "Failed to find current session!" << std::endl;
                break;
            }
            std::shared_ptr<Session> current_session = (*it).first;

            // Add response object
            // NOLINTNEXTLINE (cppcoreguidelines-pro-type-union-access)
            responses.push_back(complete_function(*current_session, info->data.result));
        }
    } while (info);

    // Sort response objects to match order of added sessions
    std::vector<Response> sorted_responses;
    for (std::pair<std::shared_ptr<Session>, HttpMethod>& pair : sessions_) {
        Session& current_session = *(pair.first);
        auto it = std::find_if(responses.begin(), responses.end(), [&current_session](const Response& response) { return current_session.curl_->handle == response.curl_->handle; });
        Response current_response = *it;
        // Erase response from original vector to increase future search speed
        responses.erase(it);
        sorted_responses.push_back(current_response);
    }

    return sorted_responses;
}

std::vector<Response> MultiPerform::MakeRequest() {
    DoMultiPerform();
    return ReadMultiInfo([](Session& session, CURLcode curl_error) -> Response { return session.Complete(curl_error); });
}

std::vector<Response> MultiPerform::MakeDownloadRequest() {
    DoMultiPerform();
    return ReadMultiInfo([](Session& session, CURLcode curl_error) -> Response { return session.CompleteDownload(curl_error); });
}

void MultiPerform::PrepareSessions() {
    for (std::pair<std::shared_ptr<Session>, HttpMethod>& pair : sessions_) {
        switch (pair.second) {
            case HttpMethod::GET_REQUEST:
                pair.first->PrepareGet();
                break;
            case HttpMethod::POST_REQUEST:
                pair.first->PreparePost();
                break;
            case HttpMethod::PUT_REQUEST:
                pair.first->PreparePut();
                break;
            case HttpMethod::DELETE_REQUEST:
                pair.first->PrepareDelete();
                break;
            case HttpMethod::PATCH_REQUEST:
                pair.first->PreparePatch();
                break;
            case HttpMethod::HEAD_REQUEST:
                pair.first->PrepareHead();
                break;
            case HttpMethod::OPTIONS_REQUEST:
                pair.first->PrepareOptions();
                break;
            default:
                std::cerr << "PrepareSessions failed: Undefined HttpMethod or download without arguments!" << std::endl;
                return;
        }
    }
}

void MultiPerform::PrepareDownloadSession(size_t sessions_index, const WriteCallback& write) {
    std::pair<std::shared_ptr<Session>, HttpMethod>& pair = sessions_[sessions_index];
    switch (pair.second) {
        case HttpMethod::DOWNLOAD_REQUEST:
            pair.first->PrepareDownload(write);
            break;
        default:
            std::cerr << "PrepareSessions failed: Undefined HttpMethod or non download method with arguments!" << std::endl;
            return;
    }
}

void MultiPerform::PrepareDownloadSession(size_t sessions_index, std::ofstream& file) {
    std::pair<std::shared_ptr<Session>, HttpMethod>& pair = sessions_[sessions_index];
    switch (pair.second) {
        case HttpMethod::DOWNLOAD_REQUEST:
            pair.first->PrepareDownload(file);
            break;
        default:
            std::cerr << "PrepareSessions failed: Undefined HttpMethod or non download method with arguments!" << std::endl;
            return;
    }
}

void MultiPerform::SetHttpMethod(HttpMethod method) {
    for (std::pair<std::shared_ptr<Session>, HttpMethod>& pair : sessions_) {
        pair.second = method;
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

} // namespace cpr