#ifndef CPR_MULTIPERFORM_H
#define CPR_MULTIPERFORM_H

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
    void AddSession(std::shared_ptr<Session>& session);
    void RemoveSession(std::shared_ptr<Session>& session);

  private:
    void PrepareGet();
    std::vector<Response> MakeRequest();
    void DoMultiPerform();
    std::vector<Response> ReadMultiInfo();

    std::vector<std::shared_ptr<Session>> sessions_;
    std::unique_ptr<CurlMultiHolder> multicurl_;
};
} // namespace cpr

#endif