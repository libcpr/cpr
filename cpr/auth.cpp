#include "auth.h"

namespace cpr {

const char* Authentication::GetAuthString() const {
    return auth_string_.data();
}

}
