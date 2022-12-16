#ifndef CURL_CONTAINER_H
#define CURL_CONTAINER_H

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

#include "cpr/curlholder.h"


namespace cpr {

struct Parameter {
    Parameter(std::string p_key, std::string p_value) : key{std::move(p_key)}, value{std::move(p_value)} {}

    std::string key;
    std::string value;
};

struct Pair {
    Pair(std::string p_key, std::string p_value) : key(std::move(p_key)), value(std::move(p_value)) {}

    std::string key;
    std::string value;
};


template <class T>
class CurlContainer {
  public:
    /**
     * Enables or disables URL encoding for keys and values when calling GetContent(...).
     **/
    bool encode = true;

    CurlContainer() = default;
    CurlContainer(const std::initializer_list<T>&);

    void Add(const std::initializer_list<T>&);
    void Add(const T&);

    const std::string GetContent(const CurlHolder&) const;

  protected:
    std::vector<T> containerList_;
};

} // namespace cpr

#endif //
