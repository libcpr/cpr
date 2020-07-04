#ifndef CPR_PARAMETERS_H
#define CPR_PARAMETERS_H

#include <initializer_list>
#include <memory>
#include <string>

#include "cpr/curlholder.h"

namespace cpr {

struct Parameter {
    Parameter(const std::string& key, const std::string& value)
            : key{key}, value{value} {}
    Parameter(std::string&& key, std::string&& value)
            : key{std::move(key)}, value{std::move(value)} {}

    std::string key;
    std::string value;
};

class Parameters {
  public:
    Parameters() = default;
    Parameters(const std::initializer_list<Parameter>& parameters);

    void AddParameter(const Parameter& parameter, const CurlHolder& holder);

    std::string content;
};

} // namespace cpr

#endif
