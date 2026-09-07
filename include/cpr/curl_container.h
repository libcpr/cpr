#ifndef CPR_CURL_CONTAINER_H
#define CPR_CURL_CONTAINER_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <initializer_list>
#include <string>
#include <vector>
#endif

#include "cpr/curlholder.h"


namespace cpr {

EXPORT_CPR struct Parameter {
    Parameter(std::string p_key, std::string p_value) : key{std::move(p_key)}, value{std::move(p_value)} {}

    std::string key;
    std::string value;
};

EXPORT_CPR struct Pair {
    Pair(std::string p_key, std::string p_value) : key(std::move(p_key)), value(std::move(p_value)) {}

    std::string key;
    std::string value;
};


EXPORT_CPR template <class T>
class CurlContainer {
  public:
    /**
     * Enables or disables URL encoding for keys and values when calling GetContent(...).
     **/
    bool encode = true;

    CurlContainer() = default;
    CurlContainer(const std::initializer_list<T>& /*containerList*/);

    void Add(const std::initializer_list<T>& /*containerList*/);
    void Add(const T& /*element*/);

    /**
     * Returns the URL using curl_easy_escape(...) for escaping the given parameters.
     * Requires `CurlHolder`.
     **/
    [[nodiscard]] const std::string GetContent(const CurlHolder& /*holder*/) const;

    /**
     * Returns the URL while ignoring `encode`. This allows calling without
     * active `CurlHolder`.
     **/
    [[nodiscard]] const std::string GetContent() const;

  protected:
    std::vector<T> containerList_;
};

} // namespace cpr

#endif // CPR_CURL_CONTAINER_H
