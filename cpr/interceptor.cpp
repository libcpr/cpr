#include "cpr/interceptor.h"

#include <exception>

namespace cpr {

Response Interceptor::proceed(Session& session) {
    return session.proceed();
}

Response Interceptor::proceed(Session& session, ProceedHttpMethod httpMethod) {
    switch (httpMethod) {
        case ProceedHttpMethod::DELETE:
            return session.Delete();
        case ProceedHttpMethod::GET:
            return session.Get();
        case ProceedHttpMethod::HEAD:
            return session.Head();
        case ProceedHttpMethod::OPTIONS:
            return session.Options();
        case ProceedHttpMethod::PATCH:
            return session.Patch();
        case ProceedHttpMethod::POST:
            return session.Post();
        case ProceedHttpMethod::PUT:
            return session.Put();
        default:
            throw std::invalid_argument{"Can't procceed the session with the provided http method!"};
    }
}

Response Interceptor::proceed(Session& session, ProceedHttpMethod httpMethod, std::ofstream& file) {
    if (httpMethod == ProceedHttpMethod::DOWNLOAD_FILE) {
        return session.Download(file);
    }
    throw std::invalid_argument{"std::ofstream argument is only valid for ProceedHttpMethod::DOWNLOAD_FILE!"};
}

Response Interceptor::proceed(Session& session, ProceedHttpMethod httpMethod, const WriteCallback& write) {
    if (httpMethod == ProceedHttpMethod::DOWNLOAD_CALLBACK) {
        return session.Download(write);
    }
    throw std::invalid_argument{"WriteCallback argument is only valid for ProceedHttpMethod::DOWNLOAD_CALLBACK!"};
}

} // namespace cpr