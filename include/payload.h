#ifndef CPR_PAYLOAD_H
#define CPR_PAYLOAD_H

#include <memory>
#include <string>
#include <initializer_list>

namespace cpr {

struct Pair {
    Pair(const std::string& key, const std::string& value) : key{key}, value{value} {}
    Pair(const std::string& key, const int& value) : key{key}, value{std::to_string(value)} {}

    std::string key;
    std::string value;
};

class Payload {
  public:
    Payload(const std::initializer_list<Pair>& pairs);
    Payload(const std::string& content) : content{content} {}

    std::string content;
};

} // namespace cpr

#endif
