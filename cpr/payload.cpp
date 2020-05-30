#include "cpr/payload.h"

#include <initializer_list>
#include <string>

#include "cpr/util.h"

namespace cpr {

Payload::Payload(const std::initializer_list<Pair>& pairs) : Payload(begin(pairs), end(pairs)) {}

void Payload::AddPair(const Pair& pair, const CurlHolder& holder) {
    if (!content.empty()) {
        content += "&";
    }
    std::string escaped = holder.urlEncode(pair.value);
    content += pair.key + "=" + escaped;
}

} // namespace cpr
