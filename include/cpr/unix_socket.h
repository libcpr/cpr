#ifndef CPR_UNIX_SOCKET_H
#define CPR_UNIX_SOCKET_H

#include <string>

namespace cpr {

class UnixSocket {
  public:
    UnixSocket(std::string&& unix_socket) : unix_socket_(std::move(unix_socket)) {}

    const char* GetUnixSocketString() const noexcept;

  private:
    const std::string unix_socket_;
};

} // namespace cpr

#endif
