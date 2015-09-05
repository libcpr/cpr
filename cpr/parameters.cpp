#include "parameters.h"

#include <initializer_list>
#include <string>

#include "util.h"

namespace cpr {

Parameters::Parameters(const std::initializer_list<Parameter>& parameters) {
    for (auto parameter = parameters.begin(); parameter != parameters.end(); ++parameter) {
        if (!content.empty()) {
            content += "&";
        }
        auto escaped = cpr::util::urlEncode(parameter->value);
        content += parameter->key + "=" + escaped;
    }
}

}
