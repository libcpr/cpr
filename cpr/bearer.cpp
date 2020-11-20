#include "cpr/bearer.h"

namespace cpr {
const char* Bearer::GetToken() const noexcept {
    return token_string_.c_str();
}
} // namespace cpr
