#ifndef CPR_CERT_INFO_H
#define CPR_CERT_INFO_H

#include <initializer_list>
#include <string>
#include <vector>

namespace cpr {

class CertInfo {
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
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    void emplace_back(const std::string& str);
    void push_back(const std::string& str);
    void pop_back();
};
} // namespace cpr

#endif
