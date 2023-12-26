#include "cpr/multipart.h"
#include <initializer_list>
#include <vector>

namespace cpr {
Multipart::Multipart(const std::initializer_list<Part>& p_parts) : parts{p_parts} {}
Multipart::Multipart(const std::vector<Part>& p_parts) : parts{p_parts} {}
Multipart::Multipart(const std::vector<Part>&& p_parts) : parts{p_parts} {}
} // namespace cpr
