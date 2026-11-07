#ifndef CPR_LOCAL_PORT_H
#define CPR_LOCAL_PORT_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <cstdint>
#endif

namespace cpr {

EXPORT_CPR class LocalPort {
  public:
    LocalPort(const std::uint16_t p_localport) : localport_(p_localport) {}

    operator std::uint16_t() const {
        return localport_;
    }

  private:
    std::uint16_t localport_ = 0;
};

} // namespace cpr

#endif
