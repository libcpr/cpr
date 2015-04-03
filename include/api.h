#ifndef CPR_API_H
#define CPR_API_H

#include <string>

#include "auth.h"
#include "cprtypes.h"
#include "response.h"


namespace cpr {
    // Get methods
    Response Get(Url url, Timeout timeout=0L);

    Response Get(Url url, Parameters parameters, Timeout timeout=0L);
    Response Get(Url url, Authentication auth, Timeout timeout=0L);
    Response Get(Url url, Header header, Timeout timeout=0L);

    Response Get(Url url, Parameters parameters, Authentication auth, Timeout timeout=0L);
    Response Get(Url url, Parameters parameters, Header header, Timeout timeout=0L);
    Response Get(Url url, Authentication auth, Parameters parameters, Timeout timeout=0L);
    Response Get(Url url, Authentication auth, Header header, Timeout timeout=0L);
    Response Get(Url url, Header header, Parameters parameters, Timeout timeout=0L);
    Response Get(Url url, Header header, Authentication auth, Timeout timeout=0L);

    Response Get(Url url, Parameters parameters, Authentication auth, Header header,
                 Timeout timeout=0L);
    Response Get(Url url, Parameters parameters, Header header, Authentication auth,
                 Timeout timeout=0L);
    Response Get(Url url, Authentication auth, Parameters parameters, Header header,
                 Timeout timeout=0L);
    Response Get(Url url, Authentication auth, Header header, Parameters parameters,
                 Timeout timeout=0L);
    Response Get(Url url, Header header, Parameters parameters, Authentication auth,
                 Timeout timeout=0L);
    Response Get(Url url, Header header, Authentication auth, Parameters parameters,
                 Timeout timeout=0L);

    // Post methods
    Response Post(Url url, Payload payload, Timeout timeout=0L);
};

#endif
