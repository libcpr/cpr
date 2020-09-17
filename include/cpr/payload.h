#ifndef CPR_PAYLOAD_H
#define CPR_PAYLOAD_H

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <string>

#include "cpr/curl_container.h"
#include <utility>

namespace cpr {


class Payload : public CurlContainer<Pair> {
  public:
    template <class It>
    Payload(const It begin, const It end) {
        for (It pair = begin; pair != end; ++pair) {
            Add(*pair);
        }
    }
    Payload(const std::initializer_list<Pair>& pairs);
};

} // namespace cpr

#endif
