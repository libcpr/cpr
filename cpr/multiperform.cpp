#include "cpr/multiperform.h"

#include <algorithm>

namespace cpr {

MultiPerform::MultiPerform() : multicurl_(new CurlMultiHolder()) {}

void MultiPerform::AddSession(std::shared_ptr<Session>& session) {
    // Add easy handle to multi handle
    CURLMcode error_code = curl_multi_add_handle(multicurl_->handle, session->curl_->handle);
    if (error_code) {
        fprintf(stderr, "curl_multi_add_handle() failed, code %d.\n", static_cast<int>(error_code));
        return;
    }

    // Lock session to the multihandle
    session->isUsedInMultiPerform = true;

    // Add session to sessions_
    sessions_.push_back(session);
}

void MultiPerform::RemoveSession(const std::shared_ptr<Session>& session) {
    // Remove easy handle from multihandle
    CURLMcode error_code = curl_multi_remove_handle(multicurl_->handle, session->curl_->handle);
    if (error_code) {
        fprintf(stderr, "curl_multi_remove_handle() failed, code %d.\n", static_cast<int>(error_code));
        return;
    }

    // Remove session from sessions_
    auto it = std::find_if(sessions_.begin(), sessions_.end(), [&session](const std::shared_ptr<Session>& current_session) { return session->curl_->handle == current_session->curl_->handle; });
    if (it == sessions_.end()) {
        fprintf(stderr, "Failed to find session!");
        return;
    }
    sessions_.erase(it);
}

void MultiPerform::DoMultiPerform() {
    // Do multi perform until every handle has finished
    int still_running{0};
    do {
        CURLMcode error_code = curl_multi_perform(multicurl_->handle, &still_running);
        if (error_code) {
            fprintf(stderr, "curl_multi_perform() failed, code %d.\n", static_cast<int>(error_code));
            break;
        }

        if (still_running) {
            const int timeout_ms{1000};
            error_code = curl_multi_poll(multicurl_->handle, nullptr, 0, timeout_ms, nullptr);
            if (error_code) {
                fprintf(stderr, "curl_multi_poll() failed, code %d.\n", static_cast<int>(error_code));
                break;
            }
        }
    } while (still_running);
}

std::vector<Response> MultiPerform::ReadMultiInfo() {
    // Get infos and create Response objects
    std::vector<Response> responses;
    struct CURLMsg* info{nullptr};
    do {
        int msgq = 0;

        // Read info from multihandle
        info = curl_multi_info_read(multicurl_->handle, &msgq);

        if (info) {
            // Find current session
            auto it = std::find_if(sessions_.begin(), sessions_.end(), [&info](const std::shared_ptr<Session>& session) { return session->curl_->handle == info->easy_handle; });
            if (it == sessions_.end()) {
                fprintf(stderr, "Failed to find current session!");
                break;
            }
            std::shared_ptr<Session> current_session = *it;

            // Add response object
            // NOLINTNEXTLINE (cppcoreguidelines-pro-type-union-access)
            responses.push_back(current_session->Complete(info->data.result));
        }
    } while (info);

    return responses;
}

std::vector<Response> MultiPerform::MakeRequest() {
    DoMultiPerform();
    return ReadMultiInfo();
}

void MultiPerform::PrepareGet() {
    for (std::shared_ptr<Session>& session : sessions_) {
        session->PrepareGet();
    }
}

std::vector<Response> MultiPerform::Get() {
    PrepareGet();
    return MakeRequest();
}

} // namespace cpr