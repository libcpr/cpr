#ifndef CPR_PAYLOAD_H
#define CPR_PAYLOAD_H

#include <memory>
#include <string>
#include <initializer_list>

#include "defines.h"

namespace cpr {

struct Pair {
    template <typename KeyType, typename ValueType,
              typename std::enable_if<!std::is_integral<ValueType>::value, bool>::type = true>
    Pair(KeyType&& key, ValueType&& value)
            : key{CPR_FWD(key)}, value{CPR_FWD(value)} {}
    template <typename KeyType>
    Pair(KeyType&& key, const int& value)
            : key{CPR_FWD(key)}, value{std::to_string(value)} {}

    std::string key;
    std::string value;
};

class Payload {
  public:
    Payload(const std::initializer_list<Pair>& pairs);

    std::string content;
};

} // namespace cpr

#endif
