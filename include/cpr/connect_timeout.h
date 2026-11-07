#ifndef CPR_CONNECT_TIMEOUT_H
#define CPR_CONNECT_TIMEOUT_H

#include "cpr/export.h"

#include "cpr/timeout.h"

namespace cpr {

EXPORT_CPR class ConnectTimeout : public Timeout {
  public:
    ConnectTimeout(const std::chrono::milliseconds& duration) : Timeout{duration} {}
    ConnectTimeout(const std::int32_t& milliseconds) : Timeout{milliseconds} {}
};

} // namespace cpr

#endif
