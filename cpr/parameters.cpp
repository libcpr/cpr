#include "parameters.h"

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

Parameters::Parameters(const std::initializer_list<Parameter>& parameters) {
    for (auto parameter = parameters.begin(); parameter != parameters.end(); ++parameter) {
        if (!content.empty()) {
            content += "&";
        }
        auto escaped = url_encode(parameter->value);
        content += parameter->key + "=" + escaped;
    }
}
