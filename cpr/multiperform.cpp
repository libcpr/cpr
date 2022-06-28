#include "cpr/multiperform.h"

namespace cpr {

MultiPerform::MultiPerform() : multicurl_(new CurlMultiHolder()) {}

void MultiPerform::AddSession(std::shared_ptr<Session>& session) {
    // Add easy handle to multi handle
    CURLMcode error_code = curl_multi_add_handle(multicurl_->handle, session->curl_->handle);
    if (error_code) {
        fprintf(stderr, "curl_multi_add_handle() failed, code %d.\n", (int) error_code);
        return;
    }

    // Add session to sessions_
    sessions_.push_back(session);
}

void MultiPerform::RemoveSession(std::shared_ptr<Session>& session) {
    // Remove easy handle from multihandle
    CURLMcode error_code = curl_multi_remove_handle(multicurl_->handle, session->curl_->handle);
    if (error_code) {
        fprintf(stderr, "curl_multi_remove_handle() failed, code %d.\n", (int) error_code);
        return;
    }

    // Remove session from sessions_
    for (auto it = sessions_.begin(); it != sessions_.end(); ++it) {
        if (session->curl_->handle == (*it)->curl_->handle) {
            sessions_.erase(it);
            break;
        }
    }
}

void MultiPerform::DoMultiPerform() {
    // Do multi perform until every handle has finished
    int still_running{0};
    CURLMcode error_code;
    do {
        error_code = curl_multi_perform(multicurl_->handle, &still_running);
        if (error_code) {
            fprintf(stderr, "curl_multi_perform() failed, code %d.\n", (int) error_code);
            break;
        }

        if (still_running) {
            error_code = curl_multi_poll(multicurl_->handle, NULL, 0, 1000, NULL);
            if (error_code) {
                fprintf(stderr, "curl_multi_poll() failed, code %d.\n", (int) error_code);
                break;
            }
        }
    } while (still_running);
}

std::vector<Response> MultiPerform::ReadMultiInfo() {
    // Get infos and create Response objects
    std::vector<Response> responses;
    struct CURLMsg* info;
    do {
        int msgq = 0;

        // Read info from multihandle
        info = curl_multi_info_read(multicurl_->handle, &msgq);

        if (info) {
            // Find current session
            std::shared_ptr<Session> current_session;
            for (std::shared_ptr<Session> session : sessions_) {
                if (session->curl_->handle == info->easy_handle) {
                    current_session = session;
                    break;
                }
            }

            // Add response object
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
    for (std::shared_ptr<Session> session : sessions_) {
        session->PrepareGet();
    }
}

std::vector<Response> MultiPerform::Get() {
    PrepareGet();
    return MakeRequest();
}

} // namespace cpr