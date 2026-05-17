#ifndef CPR_PARAMETERS_H
#define CPR_PARAMETERS_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <initializer_list>
#endif

#include "cpr/curl_container.h"

namespace cpr {

EXPORT_CPR class Parameters : public CurlContainer<Parameter> {
  public:
    Parameters() = default;
    Parameters(const std::initializer_list<Parameter>& parameters) : CurlContainer<Parameter>(parameters) {}
};

} // namespace cpr

#endif
