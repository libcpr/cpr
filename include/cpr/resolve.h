#ifndef CPR_RESOLVE_H
#define CPR_RESOLVE_H

#include <set>
#include <string>

namespace cpr {
class Resolve {
  public:
    std::string host;
    std::string addr;
    std::set<uint16_t> ports;

    Resolve(const std::string& host_param, const std::string& addr_param, const std::set<uint16_t>& ports_param = std::set<uint16_t>{80U, 443U}) : host(host_param), addr(addr_param), ports(ports_param) {
        if (this->ports.empty()) {
            this->ports.insert(80U);
            this->ports.insert(443U);
        }
    }
};
} // namespace cpr

#endif
