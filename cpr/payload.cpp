#include "cpr/payload.h"

#include <initializer_list>
#include <string>

#include "cpr/util.h"
#include "cpr/curl_container.h"

namespace cpr {
Payload::Payload(const std::initializer_list<cpr::Pair>& pairs) : CurlContainer<cpr::Pair>(pairs) {}
} // namespace cpr
