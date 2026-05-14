#ifndef CPR_PARAMETERS_H
#define CPR_PARAMETERS_H

#include "cpr/export.h"

#include <initializer_list>

#include "cpr/curl_container.h"

namespace cpr {

EXPORT_CPR class Parameters : public CurlContainer<Parameter> {
  public:
    Parameters() = default;
    Parameters(const std::initializer_list<Parameter>& parameters) : CurlContainer<Parameter>(parameters) {}
};

} // namespace cpr

#endif
