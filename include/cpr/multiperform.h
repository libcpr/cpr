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
    MultiPerform();
    std::vector<Response> Get();
    std::vector<Response> Delete();
    std::vector<Response> Download(const WriteCallback& write);
    std::vector<Response> Download(std::ofstream& file);
    std::vector<Response> Put();
    std::vector<Response> Head();
    std::vector<Response> Options();
    std::vector<Response> Patch();
    std::vector<Response> Post();
    void AddSession(std::shared_ptr<Session>& session);
    void RemoveSession(const std::shared_ptr<Session>& session);

  private:
    void PrepareSessions(std::function<void(Session&)> setup_function);
    void PrepareGet();
    void PrepareDelete();
    void PrepareDownload(const WriteCallback& write);
    void PrepareDownload(std::ofstream& file);
    void PreparePut();
    void PreparePatch();
    void PrepareHead();
    void PrepareOptions();
    void PreparePost();
    std::vector<Response> MakeRequest();
    std::vector<Response> MakeDownloadRequest();
    void DoMultiPerform();
    std::vector<Response> ReadMultiInfo(std::function<Response(Session&, CURLcode)> complete_function);

    std::vector<std::shared_ptr<Session>> sessions_;
    std::unique_ptr<CurlMultiHolder> multicurl_;
};
} // namespace cpr

#endif