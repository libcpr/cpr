#ifndef CPR_CPR_TYPES_H
#define CPR_CPR_TYPES_H

#include <cstring>
#include <initializer_list>
#include <map>
#include <numeric>
#include <string>

namespace cpr {

template <class T>
class StringHolder {
  public:
    StringHolder() {}
    explicit StringHolder(const std::string &str) : str_(str) {}
    explicit StringHolder(const char *str) : str_(std::string(str)) {}
    StringHolder(const char *str, size_t len) : str_(std::string(str, len)) {}
    StringHolder(const std::initializer_list<std::string> args) {
        str_ = std::accumulate(args.begin(), args.end(), str_);
    }

    operator std::string() const { return str_; }

    T operator+(const char *rhs) const { return T(str_ + rhs); }
    T operator+(const std::string &rhs) const { return T(str_ + rhs); }
    T operator+(const StringHolder<T> &rhs) const { return T(str_ + rhs.str_); }

    void operator+=(const char *rhs) { str_ += rhs; }
    void operator+=(const std::string &rhs) { str_ += rhs; }
    void operator+=(const StringHolder<T> &rhs) { str_ += rhs; }

    bool operator==(const char *rhs) const { return str_.compare(rhs) == 0; }
    bool operator==(const std::string &rhs) const { return str_ == rhs; }
    bool operator==(const StringHolder<T> &rhs) const { return str_ == rhs.str_; }

    bool operator!=(const char *rhs) const { return str_.c_str() != rhs; }
    bool operator!=(const std::string &rhs) const { return str_ != rhs; }
    bool operator!=(const StringHolder<T> &rhs) const { return str_ != rhs.str_; }

    std::string str() { return str_; }
    const std::string &str() const { return str_; }
    const char *c_str() const { return str_.c_str(); }
    const char *data() const { return str_.data(); }

  private:
    std::string str_ = "";
};

template <class T>
std::ostream &operator<<(std::ostream &os, const StringHolder<T> &s)
{
    os << s.str();
    return os;
}

class Url : public StringHolder<Url> {
  public:
    using StringHolder<Url>::StringHolder;
    // n.b.: This must be the only cpr type with an implicit conversion from std::string or const char*.
    // This was added for backwards compatibility when StringHolder was added.
    Url() : StringHolder<Url>() {}
    Url(const std::string &url) : StringHolder<Url>(url) {}
    Url(const char *url) : StringHolder<Url>(url) {}
};

struct CaseInsensitiveCompare {
    bool operator()(const std::string& a, const std::string& b) const noexcept;
};

using Header = std::map<std::string, std::string, CaseInsensitiveCompare>;

} // namespace cpr

#endif
