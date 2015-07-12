#ifndef PROXIES_H
#define PROXIES_H

#include <initializer_list>
#include <string>
#include <vector>


struct Proxy {
    Proxy(const std::string& protocol, const std::string& url) : protocol{protocol}, url{url} {}

    std::string protocol;
    std::string url;
};

class Proxies {
  public:
    Proxies() {}
    Proxies(const std::initializer_list<Proxy>& proxies);

    std::vector<Proxy> hosts;
};

#endif
