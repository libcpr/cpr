#include "digest.h"

namespace cpr {

const char* Digest::GetAuthString() const {
    return Authentication::GetAuthString();
}

}
