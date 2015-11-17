#include <initializer_list>
#include <string>

#include "cpr/payload.h"
#include "cpr/util.h"

namespace cpr {

Payload::Payload(const std::initializer_list<Pair>& pairs) : Payload(begin(pairs), end(pairs)) {}

void Payload::AddPair(const Pair& pair) {
    if (!content.empty()) {
        content += "&";
    }
    auto escaped = cpr::util::urlEncode(pair.value);
    content += pair.key + "=" + escaped;
}

Payload::Payload(const std::string& data) {
  content += data;
}

} // namespace cpr
