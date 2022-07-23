#include "cpr/cert_info.h"

namespace cpr {

std::string& CertInfo::operator[](const size_t& pos) {
    return cert_info_[pos];
}

CertInfo::iterator CertInfo::begin() {
    return cert_info_.begin();
}
CertInfo::iterator CertInfo::end() {
    return cert_info_.end();
}

CertInfo::const_iterator CertInfo::begin() const {
    return cert_info_.begin();
}

CertInfo::const_iterator CertInfo::end() const {
    return cert_info_.end();
}

CertInfo::const_iterator CertInfo::cbegin() const {
    return cert_info_.cbegin();
}

CertInfo::const_iterator CertInfo::cend() const {
    return cert_info_.cend();
}

void CertInfo::emplace_back(const std::string& str) {
    cert_info_.emplace_back(str);
}

void CertInfo::push_back(const std::string& str) {
    cert_info_.push_back(str);
}

void CertInfo::pop_back() {
    cert_info_.pop_back();
}
} // namespace cpr
