#ifndef CPR_INTERFACE_H_
#define CPR_INTERFACE_H_

#include <cstdbool>

namespace cpr {

    class NetworkInterface {
    public:
        NetworkInterface(const std::string network_interface) : network_interface{network_interface} {}

        std::string network_interface;
    };

} // namespace cpr

#endif /* CPR_INTERFACE_H_ */
