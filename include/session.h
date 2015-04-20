#ifndef CPR_SESSION_H
#define CPR_SESSION_H

#include <memory>

#include "auth.h"
#include "cprtypes.h"
#include "multipart.h"
#include "payload.h"
#include "response.h"


class Session {
  public:
    Session();
    ~Session();

    void SetUrl(const Url& url);
    void SetUrl(const Url& url, const Parameters& parameters);
    void SetHeader(const Header& header);
    void SetTimeout(const Timeout& timeout);
    void SetAuth(const Authentication& auth);
    void SetPayload(Payload&& payload);
    void SetPayload(const Payload& payload);
    void SetMultipart(Multipart&& multipart);
    void SetMultipart(const Multipart& multipart);
    void SetRedirect(const bool& redirect);
    void SetMaxRedirects(const long& max_redirects);
    // void SetCookie(); Unimplemented
    // void SetCookies(); Unimplemented

    Response Get();
    Response Post();

  private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

#endif
