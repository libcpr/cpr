#include "testUtils.hpp"

#include <cstdint>
#include <stdexcept>

#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace cpr::test {

std::uint16_t get_free_port() {
#ifdef _WIN32
    static const WSAInit wsa_guard; // one-time Winsock init
#endif

    // 1. Create a TCP socket.
    socket_t sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET_FD) {
        throw std::runtime_error("socket() failed");
    }

    // 2. Bind to port 0 so the OS assigns an ephemeral port.
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(0); // 0 ⇒ “pick for me”

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    if (::bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
#ifdef _WIN32
        ::closesocket(sock);
#else
        ::close(sock);
#endif
        throw std::runtime_error("bind() failed");
    }

    // 3. Ask what port we actually got.
    socklen_t len = sizeof(addr);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    if (::getsockname(sock, reinterpret_cast<sockaddr*>(&addr), &len) != 0) {
#ifdef _WIN32
        ::closesocket(sock);
#else
        ::close(sock);
#endif
        throw std::runtime_error("getsockname() failed");
    }

    std::uint16_t port = ntohs(addr.sin_port);

    // 4. Close the socket ‒ we only needed it to grab the port number.
#ifdef _WIN32
    ::closesocket(sock);
#else
    ::close(sock);
#endif

    return port;
}
} // namespace cpr::test
