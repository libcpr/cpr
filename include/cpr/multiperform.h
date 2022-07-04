#ifndef CPR_MULTIPERFORM_H
#define CPR_MULTIPERFORM_H

#include <functional>
#include <memory>
#include <vector>

#include "cpr/curlmultiholder.h"
#include "cpr/response.h"
#include "cpr/session.h"

namespace cpr {

class MultiPerform {
  public:
    enum class HttpMethod {
        UNDEFINED = 0,
        GET_REQUEST,
        POST_REQUEST,
        PUT_REQUEST,
        DELETE_REQUEST,
        PATCH_REQUEST,
        HEAD_REQUEST,
        OPTIONS_REQUEST,
        DOWNLOAD_REQUEST,
    };

    MultiPerform();
    std::vector<Response> Get();
    std::vector<Response> Delete();
    template <typename... DownloadArgTypes>
    std::vector<Response> Download(DownloadArgTypes... args);
    std::vector<Response> Put();
    std::vector<Response> Head();
    std::vector<Response> Options();
    std::vector<Response> Patch();
    std::vector<Response> Post();

    std::vector<Response> Perform();
    template <typename... DownloadArgTypes>
    std::vector<Response> PerformDownload(DownloadArgTypes... args);

    void AddSession(std::shared_ptr<Session>& session, HttpMethod method = HttpMethod::UNDEFINED);
    void RemoveSession(const std::shared_ptr<Session>& session);

  private:
    void SetHttpMethod(HttpMethod method);

    void PrepareSessions();
    template <typename CurrentDownloadArgType, typename... DownloadArgTypes>
    void PrepareDownloadSessions(size_t sessions_index, CurrentDownloadArgType current_arg, DownloadArgTypes... args);

    void PrepareGet();
    void PrepareDelete();
    void PreparePut();
    void PreparePatch();
    void PrepareHead();
    void PrepareOptions();
    void PreparePost();
    template <typename... DownloadArgTypes>
    void PrepareDownload(DownloadArgTypes... args);

    std::vector<Response> MakeRequest();
    std::vector<Response> MakeDownloadRequest();

    void DoMultiPerform();
    std::vector<Response> ReadMultiInfo(std::function<Response(Session&, CURLcode)>&& complete_function);

    std::vector<std::pair<std::shared_ptr<Session>, HttpMethod>> sessions_;
    std::unique_ptr<CurlMultiHolder> multicurl_;
    bool is_download_multi_perform{false};
};

template <typename CurrentDownloadArgType, typename... DownloadArgTypes>
void MultiPerform::PrepareDownloadSessions(size_t sessions_index, CurrentDownloadArgType current_arg, DownloadArgTypes... args) {
    if (sessions_index >= sessions_.size()) {
        return;
    }

    std::pair<std::shared_ptr<Session>, HttpMethod>& pair = sessions_[sessions_index];
    switch (pair.second) {
        case HttpMethod::DOWNLOAD_REQUEST:
            pair.first->PrepareDownload(current_arg);
            break;
        default:
            fprintf(stderr, "PrepareSessions failed: Undefined HttpMethod or non download method with arguments!\n");
            return;
    }

    PrepareDownloadSessions<DownloadArgTypes...>(sessions_index + 1, args...);
}


template <typename... DownloadArgTypes>
void MultiPerform::PrepareDownload(DownloadArgTypes... args) {
    SetHttpMethod(HttpMethod::DOWNLOAD_REQUEST);
    PrepareDownloadSessions<DownloadArgTypes...>(0, args...);
}

template <typename... DownloadArgTypes>
std::vector<Response> MultiPerform::Download(DownloadArgTypes... args) {
    PrepareDownload(args...);
    return MakeDownloadRequest();
}

template <typename... DownloadArgTypes>
std::vector<Response> MultiPerform::PerformDownload(DownloadArgTypes... args) {
    PrepareDownloadSessions<DownloadArgTypes...>(0, args...);
    return MakeDownloadRequest();
}

} // namespace cpr

#endif