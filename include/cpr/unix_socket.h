#ifndef CPR_UNIX_SOCKET_H
#define CPR_UNIX_SOCKET_H

#include <string>

#include "cpr/defines.h"

namespace cpr {

class UnixSocket {
  public:
    template <typename UnixSocketType>
    UnixSocket(UnixSocketType&& unix_socket)
            : unix_socket_{CPR_FWD(unix_socket)} {}

    const char* GetUnixSocketString() const noexcept;

  private:
    std::string unix_socket_;
};

} // namespace cpr

#endif
