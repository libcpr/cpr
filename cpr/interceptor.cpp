#include "cpr/interceptor.h"

#include <exception>

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
            throw std::invalid_argument{"Can't procceed the session with the provided http method!"};
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

} // namespace cpr