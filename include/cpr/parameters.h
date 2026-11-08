#ifndef CPR_PARAMETERS_H
#define CPR_PARAMETERS_H

#include <initializer_list>
#include <map>
#include <utility>

#include "cpr/curl_container.h"

namespace cpr {

class Parameters : public CurlContainer<Parameter> {
  public:
    Parameters() = default;
    Parameters(const std::initializer_list<Parameter>& parameters) : CurlContainer<Parameter>(parameters) {}

    explicit Parameters(const std::map<std::string, std::string>& parameters) {
        containerList_.reserve(parameters.size());

        for (const auto& [key, value] : parameters) {
            this->Add(Parameter{key, value});
        }
    }

    explicit Parameters(std::map<std::string, std::string>&& parameters) {
        auto owned_parameters = std::move(parameters);
        containerList_.reserve(owned_parameters.size());

        for (auto& [key, value] : owned_parameters) {
            this->Add(Parameter{key, std::move(value)});
        }
    }
};

} // namespace cpr

#endif
