#ifndef CPR_SESSION_H
#define CPR_SESSION_H

#include <memory>

#include "auth.h"
#include "cookies.h"
#include "cprtypes.h"
#include "digest.h"
#include "multipart.h"
#include "parameters.h"
#include "payload.h"
#include "proxies.h"
#include "response.h"
#include "timeout.h"

namespace cpr {

    class Session {
      public:
        Session();
        ~Session();

        void SetUrl(const Url& url);
        void SetParameters(const Parameters& parameters);
        void SetParameters(Parameters&& parameters);
        void SetHeader(const Header& header);
        void SetTimeout(const Timeout& timeout);
        void SetAuth(const Authentication& auth);
        void SetDigest(const Digest& auth);
        void SetPayload(Payload&& payload);
        void SetPayload(const Payload& payload);
        void SetProxies(Proxies&& proxies);
        void SetProxies(const Proxies& proxies);
        void SetMultipart(Multipart&& multipart);
        void SetMultipart(const Multipart& multipart);
        void SetRedirect(const bool& redirect);
        void SetMaxRedirects(const long& max_redirects);
        void SetCookies(const Cookies& cookies);

        // Used in templated functions
        void SetOption(const Url& url);
        void SetOption(const Parameters& parameters);
        void SetOption(Parameters&& parameters);
        void SetOption(const Header& header);
        void SetOption(const Timeout& timeout);
        void SetOption(const Authentication& auth);
        void SetOption(const Digest& auth);
        void SetOption(Payload&& payload);
        void SetOption(const Payload& payload);
        void SetOption(Proxies&& proxies);
        void SetOption(const Proxies& proxies);
        void SetOption(Multipart&& multipart);
        void SetOption(const Multipart& multipart);
        void SetOption(const bool& redirect);
        void SetOption(const long& max_redirects);
        void SetOption(const Cookies& cookies);

        Response Delete();
        Response Get();
        Response Head();
        Response Post();
        Response Put();

      private:
        class Impl;
        std::unique_ptr<Impl> pimpl_;
    };

} // namespace cpr

#endif
