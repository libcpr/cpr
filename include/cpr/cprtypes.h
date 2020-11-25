#ifndef CPR_CPR_TYPES_H
#define CPR_CPR_TYPES_H

#include <curl/curl.h>
#include <initializer_list>
#include <map>
#include <memory>
#include <numeric>
#include <string>

namespace cpr {

/**
 * Wrapper around "curl_off_t" to prevent applications from having to link against libcurl.
 **/
using cpr_off_t = curl_off_t;

template <class T>
class StringHolder {
  public:
    StringHolder() = default;
    explicit StringHolder(const std::string& str) : str_(str) {}
    explicit StringHolder(std::string&& str) : str_(std::move(str)) {}
    explicit StringHolder(const char* str) : str_(str) {}
    StringHolder(const char* str, size_t len) : str_(str, len) {}
    StringHolder(const std::initializer_list<std::string> args) {
        str_ = std::accumulate(args.begin(), args.end(), str_);
    }
    StringHolder(const StringHolder& other) = default;
    StringHolder(StringHolder&& old) noexcept = default;
    virtual ~StringHolder() = default;

    StringHolder& operator=(StringHolder&& old) noexcept = default;

    StringHolder& operator=(const StringHolder& other) = default;

    explicit operator std::string() const {
        return str_;
    }

    T operator+(const char* rhs) const {
        return T(str_ + rhs);
    }

    T operator+(const std::string& rhs) const {
        return T(str_ + rhs);
    }

    T operator+(const StringHolder<T>& rhs) const {
        return T(str_ + rhs.str_);
    }

    void operator+=(const char* rhs) {
        str_ += rhs;
    }
    void operator+=(const std::string& rhs) {
        str_ += rhs;
    }
    void operator+=(const StringHolder<T>& rhs) {
        str_ += rhs;
    }

    bool operator==(const char* rhs) const {
        return str_ == rhs;
    }
    bool operator==(const std::string& rhs) const {
        return str_ == rhs;
    }
    bool operator==(const StringHolder<T>& rhs) const {
        return str_ == rhs.str_;
    }

    bool operator!=(const char* rhs) const {
        return str_.c_str() != rhs;
    }
    bool operator!=(const std::string& rhs) const {
        return str_ != rhs;
    }
    bool operator!=(const StringHolder<T>& rhs) const {
        return str_ != rhs.str_;
    }

    const std::string& str() {
        return str_;
    }
    const std::string& str() const {
        return str_;
    }
    const char* c_str() const {
        return str_.c_str();
    }
    const char* data() const {
        return str_.data();
    }

  protected:
    std::string str_{};
};

template <class T>
std::ostream& operator<<(std::ostream& os, const StringHolder<T>& s) {
    os << s.str();
    return os;
}

class Url : public StringHolder<Url> {
  public:
    Url() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Url(const std::string& url) : StringHolder<Url>(url) {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Url(std::string&& url) : StringHolder<Url>(std::move(url)) {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Url(const char* url) : StringHolder<Url>(url) {}
    Url(const char* str, size_t len) : StringHolder<Url>(std::string(str, len)) {}
    Url(const std::initializer_list<std::string> args) : StringHolder<Url>(args) {}
    Url(const Url& other) = default;
    Url(Url&& old) noexcept = default;
    ~Url() override = default;

    Url& operator=(Url&& old) noexcept = default;
    Url& operator=(const Url& other) = default;
};

struct CaseInsensitiveCompare {
    bool operator()(const std::string& a, const std::string& b) const noexcept;
};

using Header = std::map<std::string, std::string, CaseInsensitiveCompare>;

} // namespace cpr

#endif
