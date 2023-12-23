#include "cpr/interceptor.h"
#include "cpr/callback.h"
#include "cpr/multiperform.h"
#include "cpr/response.h"
#include "cpr/session.h"
#include <cstddef>
#include <iosfwd>
#include <stdexcept>
#include <vector>

namespace cpr {

Response Interceptor::proceed(Session& session) {
    return session.proceed();
}

Response Interceptor::proceed(Session& session, ProceedHttpMethod httpMethod) {
    switch (httpMethod) {
        case ProceedHttpMethod::DELETE_REQUEST:
            return session.Delete();
        case ProceedHttpMethod::GET_REQUEST:
            return session.Get();
        case ProceedHttpMethod::HEAD_REQUEST:
            return session.Head();
        case ProceedHttpMethod::OPTIONS_REQUEST:
            return session.Options();
        case ProceedHttpMethod::PATCH_REQUEST:
            return session.Patch();
        case ProceedHttpMethod::POST_REQUEST:
            return session.Post();
        case ProceedHttpMethod::PUT_REQUEST:
            return session.Put();
        default:
            throw std::invalid_argument{"Can't proceed the session with the provided http method!"};
    }
}

Response Interceptor::proceed(Session& session, ProceedHttpMethod httpMethod, std::ofstream& file) {
    if (httpMethod == ProceedHttpMethod::DOWNLOAD_FILE_REQUEST) {
        return session.Download(file);
    }
    throw std::invalid_argument{"std::ofstream argument is only valid for ProceedHttpMethod::DOWNLOAD_FILE!"};
}

Response Interceptor::proceed(Session& session, ProceedHttpMethod httpMethod, const WriteCallback& write) {
    if (httpMethod == ProceedHttpMethod::DOWNLOAD_CALLBACK_REQUEST) {
        return session.Download(write);
    }
    throw std::invalid_argument{"WriteCallback argument is only valid for ProceedHttpMethod::DOWNLOAD_CALLBACK!"};
}

std::vector<Response> InterceptorMulti::proceed(MultiPerform& multi) {
    return multi.proceed();
}

void InterceptorMulti::PrepareDownloadSession(MultiPerform& multi, size_t sessions_index, const WriteCallback& write) {
    multi.PrepareDownloadSessions(sessions_index, write);
}
} // namespace cpr
