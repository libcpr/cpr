#include "cpr/parameters.h"

#include <initializer_list>
#include <string>

#include "cpr/curl_container.h"
#include "cpr/util.h"

namespace cpr {
Parameters::Parameters(const std::initializer_list<cpr::Parameter>& parameters) : CurlContainer<cpr::Parameter>(parameters) {}
} // namespace cpr
