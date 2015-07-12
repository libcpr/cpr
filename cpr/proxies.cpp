#include "proxies.h"

#include <initializer_list>


Proxies::Proxies(const std::initializer_list<Proxy>& proxies) {
    for (auto& proxy : proxies) {
        hosts.push_back(proxy);
    }
}
