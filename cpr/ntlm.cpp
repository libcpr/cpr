#include "cpr/ntlm.h"

namespace cpr {

const char* NTLM::GetAuthString() const noexcept {
    return Authentication::GetAuthString();
}

} // namespace cpr
