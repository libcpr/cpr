#include "cpr/parameters.h"

#include <initializer_list>
#include <string>

#include "cpr/util.h"

namespace cpr {

Parameters::Parameters(const std::initializer_list<Parameter>& parameters) {
    for (const auto& parameter : parameters) {
        AddParameter(parameter);
    }
}

void Parameters::AddParameter(const Parameter& parameter) {
    if (!content.empty()) {
        content += "&";
    }
    if (parameter.value.empty()) {
      content += parameter.key;
    } else {
      auto escaped = cpr::util::urlEncode(parameter.value);
      content += parameter.key + "=" + escaped;
    }
}

} // namespace cpr
