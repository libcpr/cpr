#ifndef CPR_UNIX_SOCKET_H
#define CPR_UNIX_SOCKET_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <string>
#endif

namespace cpr {

EXPORT_CPR class UnixSocket {
  public:
    UnixSocket(std::string unix_socket) : unix_socket_(std::move(unix_socket)) {}

    [[nodiscard]]
    const char* GetUnixSocketString() const noexcept;

  private:
    const std::string unix_socket_;
};

} // namespace cpr

#endif
