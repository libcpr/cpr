#ifndef CPR_USERAGENT_H
#define CPR_USERAGENT_H

#include <initializer_list>
#include <string>

namespace cpr {

class UserAgent : public std::string {
  public:
    UserAgent() = default;
    UserAgent(const UserAgent&) = default;
    UserAgent(UserAgent&&) = default;

    explicit UserAgent(const char* raw_string) : std::string(raw_string) {}
    explicit UserAgent(const char* raw_string, size_t length) : std::string(raw_string, length) {}
    explicit UserAgent(size_t to_fill, char character) : std::string(to_fill, character) {}
    explicit UserAgent(const std::string& std_string) : std::string(std_string) {}
    explicit UserAgent(const std::string& std_string, size_t position,
                       size_t length = std::string::npos)
            : std::string(std_string, position, length) {}
    explicit UserAgent(std::string&& std_string) : std::string(std::move(std_string)) {}
    explicit UserAgent(std::initializer_list<char> il) : std::string(il) {}
    template <class InputIterator>
    explicit UserAgent(InputIterator first, InputIterator last) : std::string(first, last) {}

    UserAgent& operator=(const UserAgent&) = default;
    UserAgent& operator=(UserAgent&&) = default;
};

} // namespace cpr

#endif
