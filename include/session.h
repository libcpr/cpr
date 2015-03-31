#ifndef _CPR_SESSION_H_
#define _CPR_SESSION_H_

#include <functional>
#include <memory>

#include <curl/curl.h>

#include "auth.h"
#include "cprtypes.h"
#include "response.h"


class Session {
  public:
    Session();

    void SetUrl(Url url);
    void SetUrl(Url url, Parameters parameters);
    void SetHeader(Header header);
    void SetTimeout(Timeout timeout);
    void SetAuth(Authentication auth);
    void SetPayload(Payload payload);
    void SetRedirect(bool redirect);
    // void SetCookie(); Unimplemented
    // void SetCookies(); Unimplemented

    Response Get();
    Response Post();

  private:
    std::unique_ptr<CurlHolder, std::function<void(CurlHolder*)>> curl_;
};

#endif
