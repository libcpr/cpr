
#include "cpr/unix_socket.h"

namespace cpr {
const char* UnixSocket::GetUnixSocketString() const noexcept {
    return unix_socket_.data();
}
} // namespace cpr
