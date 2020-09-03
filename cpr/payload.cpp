#include "cpr/payload.h"

#include <initializer_list>
#include <string>

#include "cpr/util.h"

namespace cpr {

Payload::Payload(const std::initializer_list<Pair>& pairs) : CurlContainer<Pair>(pairs) {

}

} // namespace cpr
