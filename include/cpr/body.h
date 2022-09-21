#ifndef CPR_BODY_H
#define CPR_BODY_H

#include <exception>
#include <initializer_list>
#include <string>
#include <vector>
#include <fstream>

#include "cpr/buffer.h"
#include "cpr/cprtypes.h"
#include "cpr/file.h"

namespace cpr {

class Body : public StringHolder<Body> {
  public:
    Body() : StringHolder<Body>() {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Body(const std::string& body) : StringHolder<Body>(body) {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Body(std::string&& body) : StringHolder<Body>(std::move(body)) {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Body(std::string_view body) : StringHolder<Body>(body) {}
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Body(const char* body) : StringHolder<Body>(body) {}
    Body(const char* str, size_t len) : StringHolder<Body>(str, len) {}
    Body(const std::initializer_list<std::string> args) : StringHolder<Body>(args) {}
    Body(const Buffer& buffer) : StringHolder<Body>(reinterpret_cast<const char*>(buffer.data), static_cast<size_t>(buffer.datalen)) {}
    Body(const File& file) {
        std::ifstream is(file.filepath, std::ifstream::binary);
        if (!is) {
            throw std::invalid_argument("Can't open the file for HTTP request body!");
        }

        is.seekg(0, std::ios::end);
        const std::streampos length = is.tellg();
        is.seekg(0, std::ios::beg);
        std::string buffer;
        buffer.resize(static_cast<size_t>(length));
        is.read(&buffer[0], length);
        str_ = std::move(buffer);
    }
    Body(const Body& other) = default;
    Body(Body&& old) noexcept = default;
    ~Body() override = default;

    Body& operator=(Body&& old) noexcept = default;
    Body& operator=(const Body& other) = default;
};

} // namespace cpr

#endif
