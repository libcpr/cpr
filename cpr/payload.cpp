#include "payload.h"

#include <initializer_list>
#include <string>

#include "util.h"

namespace cpr {

    Payload::Payload(const std::initializer_list<Pair>& pairs) {
        for (auto pair = pairs.begin(); pair != pairs.end(); ++pair) {
            if (!content.empty()) {
                content += "&";
            }
            auto escaped = cpr::util::urlEncode(pair->value);
            content += pair->key + "=" + escaped;
        }
    }

}
