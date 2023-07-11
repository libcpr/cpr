#ifndef CPR_PAYLOAD_H
#define CPR_PAYLOAD_H

#include <initializer_list>

#include "cpr/curl_container.h"


namespace cpr {
class Payload : public CurlContainer<Pair> {
  public:
    template <class It>
    Payload(const It begin, const It end) {
        for (It pair = begin; pair != end; ++pair) {
            Add(*pair);
        }
    }
    Payload(const std::initializer_list<Pair>& pairs) : CurlContainer<Pair>(pairs) {}
};

} // namespace cpr

#endif
