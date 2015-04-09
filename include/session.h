#ifndef CPR_SESSION_H
#define CPR_SESSION_H

#include <memory>

#include "auth.h"
#include "cprtypes.h"
#include "payload.h"
#include "response.h"


class Session {
  public:
    Session();
    ~Session();

    void SetUrl(Url url);
    void SetUrl(Url url, Parameters parameters);
    void SetHeader(Header header);
    void SetTimeout(Timeout timeout);
    void SetAuth(Authentication auth);
    void SetPayload(Payload payload);
    void SetRedirect(bool redirect);
    void SetMaxRedirects(long max_redirects);
    // void SetCookie(); Unimplemented
    // void SetCookies(); Unimplemented

    Response Get();
    Response Post();

  private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

#endif
