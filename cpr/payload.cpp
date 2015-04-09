#include "payload.h"

#include <cctype>
#include <initializer_list>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>


std::string url_encode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (auto i = value.cbegin(), n = value.cend(); i != n; ++i) {
        std::string::value_type c = (*i);
        // Keep alphanumeric and other accepted characters intact
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }
        // Any other characters are percent-encoded
        escaped << '%' << std::setw(2) << int((unsigned char) c);
    }

    return escaped.str();
}

Payload::Payload(const std::initializer_list<Pair>& pairs) {
    for (auto pair = pairs.begin(); pair != pairs.end(); ++pair) {
        if (!content.empty()) {
            content += "&";
        }
        auto escaped = url_encode(pair->value);
        content += pair->key + "=" + escaped;
    }
}
