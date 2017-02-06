#include "cpr/multipart.h"

namespace cpr {

Multipart::Multipart(const std::initializer_list<Part>& parts) : parts{parts} {}

void Multipart::AddPart(const Part& part) {
    parts.push_back(part);
}

} // namespace cpr
