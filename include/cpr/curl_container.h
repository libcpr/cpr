#ifndef CURL_CONTAINER_H
#define CURL_CONTAINER_H

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

#include "cpr/curlholder.h"


namespace cpr {

struct Parameter {
    Parameter(const std::string& key, const std::string& value)
            : key{key}, value{value} {}
    Parameter(std::string&& key, std::string&& value)
            : key{std::move(key)}, value{std::move(value)} {}

    std::string key;
    std::string value;
};

struct Pair {
    Pair(const std::string& p_key, const std::string& p_value)
            : key(p_key), value(p_value) {}
    Pair(std::string&& p_key, std::string&& p_value)
            : key(std::move(p_key)), value(std::move(p_value)) {}

    std::string key;
    std::string value;
};


template<class T>
class CurlContainer {
  public:
    CurlContainer() = default;
    CurlContainer(const std::initializer_list<T>&);

  public:
    void Add(const std::initializer_list<T>&);
    void Add(const T&);

  public:
    const std::string GetContent(const CurlHolder&) const;

  protected:
    std::vector<T> containerList_;
};

} // namespace cpr

#endif //
