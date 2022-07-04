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

    // Unock session
    session->isUsedInMultiPerform = false;

    // Remove session from sessions_
    auto it = std::find_if(sessions_.begin(), sessions_.end(), [&session](const std::shared_ptr<Session>& current_session) { return session->curl_->handle == current_session->curl_->handle; });
    if (it == sessions_.end()) {
        fprintf(stderr, "Failed to find session!\n");
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

std::vector<Response> MultiPerform::ReadMultiInfo(std::function<Response(Session&, CURLcode)> complete_function) {
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
                fprintf(stderr, "Failed to find current session!\n");
                break;
            }
            std::shared_ptr<Session> current_session = *it;

            // Add response object
            // NOLINTNEXTLINE (cppcoreguidelines-pro-type-union-access)
            responses.push_back(complete_function(*current_session, info->data.result));
        }
    } while (info);

    return responses;
}

std::vector<Response> MultiPerform::MakeRequest() {
    DoMultiPerform();
    return ReadMultiInfo([](Session& session, CURLcode curl_error) -> Response { return session.Complete(curl_error); });
}

std::vector<Response> MultiPerform::MakeDownloadRequest() {
    DoMultiPerform();
    return ReadMultiInfo([](Session& session, CURLcode curl_error) -> Response { return session.CompleteDownload(curl_error); });
}

void MultiPerform::PrepareSessions(std::function<void(Session&)> setup_function) {
    for (std::shared_ptr<Session>& session : sessions_) {
        setup_function(*session);
    }
}

void MultiPerform::PrepareGet() {
    PrepareSessions([](Session& session) { session.PrepareGet(); });
}

void MultiPerform::PrepareDelete() {
    PrepareSessions([](Session& session) { session.PrepareDelete(); });
}

void MultiPerform::PrepareDownload(const WriteCallback& write) {
    PrepareSessions([&write](Session& session) { session.PrepareDownload(write); });
}

void MultiPerform::PrepareDownload(std::ofstream& file) {
    PrepareSessions([&file](Session& session) { session.PrepareDownload(file); });
}

void MultiPerform::PreparePut() {
    PrepareSessions([](Session& session) { session.PreparePut(); });
}

void MultiPerform::PreparePatch() {
    PrepareSessions([](Session& session) { session.PreparePatch(); });
}

void MultiPerform::PrepareHead() {
    PrepareSessions([](Session& session) { session.PrepareHead(); });
}

void MultiPerform::PrepareOptions() {
    PrepareSessions([](Session& session) { session.PrepareOptions(); });
}

void MultiPerform::PreparePost() {
    PrepareSessions([](Session& session) { session.PreparePost(); });
}

std::vector<Response> MultiPerform::Get() {
    PrepareGet();
    return MakeRequest();
}

std::vector<Response> MultiPerform::Delete() {
    PrepareDelete();
    return MakeRequest();
}

std::vector<Response> MultiPerform::Download(const WriteCallback& write) {
    PrepareDownload(write);
    return MakeDownloadRequest();
}

std::vector<Response> MultiPerform::Download(std::ofstream& file) {
    PrepareDownload(file);
    return MakeDownloadRequest();
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

} // namespace cpr