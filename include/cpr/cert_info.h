#ifndef CPR_CERT_INFO_H
#define CPR_CERT_INFO_H

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

namespace cpr {

EXPORT_CPR class CertInfo {
  private:
    std::vector<std::string> cert_info_;

  public:
    CertInfo() = default;
    CertInfo(const CertInfo& other) = default;
    CertInfo(CertInfo&& old) = default;
    CertInfo(const std::initializer_list<std::string>& entry) : cert_info_{entry} {}
    ~CertInfo() noexcept = default;

    using iterator = std::vector<std::string>::iterator;
    using const_iterator = std::vector<std::string>::const_iterator;

    std::string& operator[](const size_t& pos);
    iterator begin();
    iterator end();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;
    [[nodiscard]] const_iterator cbegin() const;
    [[nodiscard]] const_iterator cend() const;
    void emplace_back(const std::string& str);
    void push_back(const std::string& str);
    void pop_back();
};
} // namespace cpr

#endif
