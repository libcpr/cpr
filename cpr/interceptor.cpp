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

template <typename T>
Response Interceptor::proceed(Session& session, ProceedHttpMethod httpMethod, T&& arg) {
    switch (httpMethod) {
        case ProceedHttpMethod::DOWNLOAD_CALLBACK:
        case ProceedHttpMethod::DOWNLOAD_FILE:
            return session.Download(arg);
        default:
            throw std::invalid_argument{"Provided argument for non DOWNLOAD http method!"};
    }
}

template Response Interceptor::proceed<WriteCallback>(Session& session, ProceedHttpMethod httpMethod, WriteCallback&& arg);
template Response Interceptor::proceed<std::ofstream>(Session& session, ProceedHttpMethod httpMethod, std::ofstream&& arg);

} // namespace cpr