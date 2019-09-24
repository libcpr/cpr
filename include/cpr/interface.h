#ifndef CPR_INTERFACE_H_
#define CPR_INTERFACE_H_

#include <cstdbool>

namespace cpr {

    class Interface {
    public:
        Interface(const std::string interface) : interface{interface} {}

        std::string interface;
    };

} // namespace cpr

#endif /* CPR_INTERFACE_H_ */
