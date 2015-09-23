#ifndef CPR_DIGEST_H
#define CPR_DIGEST_H

#include <utility>

#include "auth.h"

namespace cpr {

    class Digest : public Authentication {
      public:
        template<typename UserType, typename PassType>
        Digest(UserType&& username, PassType&& password)
            : Authentication{std::forward<decltype(username)>(username),
                             std::forward<decltype(password)>(password)} {}

        const char* GetAuthString() const;
    };

}

#endif
