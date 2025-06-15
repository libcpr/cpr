#ifndef CPR_TEST_UTILS_H
#define CPR_TEST_UTILS_H
#include <cstdint>
#include <stdexcept>

#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

namespace cpr::test {

#ifdef _WIN32
using socket_t = SOCKET;
constexpr socket_t INVALID_SOCKET_FD = INVALID_SOCKET;

struct WSAInit {
    WSAInit() {
        WSADATA data;
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }
    ~WSAInit() {
        WSACleanup();
    }
};
#else
using socket_t = int;
constexpr socket_t INVALID_SOCKET_FD = -1;
#endif

/// Return a currently unused TCP port.
/// \throws std::runtime_error on failure.
uint16_t get_free_port();
} // namespace cpr::test
#endif