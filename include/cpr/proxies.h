#ifndef CPR_PROXIES_H
#define CPR_PROXIES_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <initializer_list>
#include <map>
#include <string>
#endif

namespace cpr {
EXPORT_CPR class Proxies {
  public:
    Proxies() = default;
    Proxies(const std::initializer_list<std::pair<const std::string, std::string>>& hosts);
    explicit Proxies(const std::map<std::string, std::string>& hosts);

    [[nodiscard]] bool has(const std::string& protocol) const;
    const std::string& operator[](const std::string& protocol);

  private:
    std::map<std::string, std::string> hosts_;
};
} // namespace cpr

#endif
