#include "proxies.h"

#include <initializer_list>
#include <map>
#include <string>
#include <utility>

namespace cpr {

    Proxies::Proxies(const std::initializer_list<std::pair<std::string, std::string>>& hosts) {
        for (auto& host : hosts) {
            hosts_[host.first] = host.second;
        }
    }

    bool Proxies::has(const std::string& protocol) {
        return hosts_.count(protocol) > 0;
    }

    const std::string& Proxies::operator[](const std::string& protocol) {
        return hosts_[protocol];
    }

}
