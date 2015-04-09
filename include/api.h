#ifndef CPR_API_H
#define CPR_API_H

#include <string>

#include "auth.h"
#include "cprtypes.h"
#include "payload.h"
#include "response.h"


namespace cpr {
    // Get methods
    Response Get(const Url& url, const Timeout& timeout=0L);

    Response Get(const Url& url, const Parameters& parameters, const Timeout& timeout=0L);
    Response Get(const Url& url, const Authentication& auth, const Timeout& timeout=0L);
    Response Get(const Url& url, const Header& header, const Timeout& timeout=0L);

    Response Get(const Url& url, const Parameters& parameters, const Authentication& auth,
                 const Timeout& timeout=0L);
    Response Get(const Url& url, const Parameters& parameters, const Header& header,
                 const Timeout& timeout=0L);
    Response Get(const Url& url, const Authentication& auth, const Parameters& parameters,
                 const Timeout& timeout=0L);
    Response Get(const Url& url, const Authentication& auth, const Header& header,
                 const Timeout& timeout=0L);
    Response Get(const Url& url, const Header& header, const Parameters& parameters,
                 const Timeout& timeout=0L);
    Response Get(const Url& url, const Header& header, const Authentication& auth,
                 const Timeout& timeout=0L);

    Response Get(const Url& url, const Parameters& parameters, const Authentication& auth,
                 const Header& header, const Timeout& timeout=0L);
    Response Get(const Url& url, const Parameters& parameters, const Header& header,
                 const Authentication& auth, const Timeout& timeout=0L);
    Response Get(const Url& url, const Authentication& auth, const Parameters& parameters,
                 const Header& header, const Timeout& timeout=0L);
    Response Get(const Url& url, const Authentication& auth, const Header& header,
                 const Parameters& parameters, const Timeout& timeout=0L);
    Response Get(const Url& url, const Header& header, const Parameters& parameters,
                 const Authentication& auth, const Timeout& timeout=0L);
    Response Get(const Url& url, const Header& header, const Authentication& auth,
                 const Parameters& parameters, const Timeout& timeout=0L);

    // Post methods
    Response Post(const Url& url, const Payload& payload, const Timeout& timeout=0L);
};

#endif
