#ifndef CPR_RESOLVE_H
#define CPR_RESOLVE_H

#include <string>

namespace cpr {
    class Resolve {
      public:
        std::string host;
        std::string addr;
        std::vector<unsigned> ports;

        Resolve(const std::string& host_param, const std::string& addr_param, const std::vector<unsigned>& ports_param = std::vector<unsigned>{80U, 443U}): host(host_param), addr(addr_param), ports(ports_param) {
            if (this->ports.empty()) {
                this->ports.push_back(80U);
                this->ports.push_back(443U);
            }
        }
    };
} // namespace cpr

#endif
