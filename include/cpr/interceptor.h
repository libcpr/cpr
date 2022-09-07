#ifndef CPR_INTERCEPTOR_H
#define CPR_INTERCEPTOR_H

#include "cpr/response.h"
#include "cpr/session.h"

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

    virtual ~Interceptor() = default;
    virtual Response intercept(Session& session) = 0;

  protected:
    static Response proceed(Session& session);
    static Response proceed(Session& session, ProceedHttpMethod httpMethod);
    static Response proceed(Session& session, ProceedHttpMethod httpMethod, std::ofstream& file);
    static Response proceed(Session& session, ProceedHttpMethod httpMethod, const WriteCallback& write);
};

} // namespace cpr


#endif