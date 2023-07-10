#ifndef CPR_INTERCEPTOR_H
#define CPR_INTERCEPTOR_H

#include "cpr/multiperform.h"
#include "cpr/response.h"
#include "cpr/session.h"
#include <vector>

namespace cpr {
class Interceptor {
  public:
    enum class ProceedHttpMethod {
        GET_REQUEST = 0,
        POST_REQUEST,
        PUT_REQUEST,
        DELETE_REQUEST,
        PATCH_REQUEST,
        HEAD_REQUEST,
        OPTIONS_REQUEST,
        DOWNLOAD_CALLBACK_REQUEST,
        DOWNLOAD_FILE_REQUEST,
    };

    Interceptor() = default;
    Interceptor(const Interceptor& other) = default;
    Interceptor(Interceptor&& old) = default;
    virtual ~Interceptor() = default;

    Interceptor& operator=(const Interceptor& other) = default;
    Interceptor& operator=(Interceptor&& old) = default;

    virtual Response intercept(Session& session) = 0;

  protected:
    static Response proceed(Session& session);
    static Response proceed(Session& session, ProceedHttpMethod httpMethod);
    static Response proceed(Session& session, ProceedHttpMethod httpMethod, std::ofstream& file);
    static Response proceed(Session& session, ProceedHttpMethod httpMethod, const WriteCallback& write);
};

class InterceptorMulti {
  public:
    enum class ProceedHttpMethod {
        GET_REQUEST = 0,
        POST_REQUEST,
        PUT_REQUEST,
        DELETE_REQUEST,
        PATCH_REQUEST,
        HEAD_REQUEST,
        OPTIONS_REQUEST,
        DOWNLOAD_CALLBACK_REQUEST,
        DOWNLOAD_FILE_REQUEST,
    };

    InterceptorMulti() = default;
    InterceptorMulti(const InterceptorMulti& other) = default;
    InterceptorMulti(InterceptorMulti&& old) = default;
    virtual ~InterceptorMulti() = default;

    InterceptorMulti& operator=(const InterceptorMulti& other) = default;
    InterceptorMulti& operator=(InterceptorMulti&& old) = default;

    virtual std::vector<Response> intercept(MultiPerform& multi) = 0;

  protected:
    static std::vector<Response> proceed(MultiPerform& multi);

    static void PrepareDownloadSession(MultiPerform& multi, size_t sessions_index, const WriteCallback& write);
};

} // namespace cpr


#endif
