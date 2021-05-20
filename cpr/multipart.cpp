#include "cpr/multipart.h"

namespace cpr {
Multipart::Multipart(const std::initializer_list<Part>& p_parts) : parts{p_parts} {}
} // namespace cpr
