#ifndef CPR_MULTIPART_H
#define CPR_MULTIPART_H

#include <cstdint>
#include <initializer_list>
#include <string>
#include <type_traits>
#include <vector>

#include "buffer.h"
#include "file.h"

namespace cpr {

struct Part {
    Part(const std::string& p_name, const std::string& p_value, const std::string& p_content_type = {}) : name{p_name}, value{p_value}, content_type{p_content_type}, is_file{false}, is_buffer{false}, has_filename{false} {}
    Part(const std::string& p_name, const std::int32_t& p_value, const std::string& p_content_type = {}) : name{p_name}, value{std::to_string(p_value)}, content_type{p_content_type}, is_file{false}, is_buffer{false}, has_filename{false} {}
    Part(const std::string& p_name, const File& file, const std::string& p_content_type = {}) : name{p_name}, value{file.filepath}, content_type{p_content_type}, is_file{true}, is_buffer{false}, has_filename{false} {}
    Part(const std::string& p_name, const std::string& p_filename, const File& file, const std::string& p_content_type = {}) : name{p_name}, filename{p_filename}, value{file.filepath}, content_type{p_content_type}, is_file{true}, is_buffer{false}, has_filename{true} {}
    Part(const std::string& p_name, const Buffer& buffer, const std::string& p_content_type = {}) : name{p_name}, value{buffer.filename}, content_type{p_content_type}, data{buffer.data}, datalen{buffer.datalen}, is_file{false}, is_buffer{true}, has_filename{false} {}

    std::string name;
    std::string filename;
    std::string value;
    std::string content_type;
    Buffer::data_t data{nullptr};
    // Ignored here since libcurl reqires a long:
    // NOLINTNEXTLINE(google-runtime-int)
    long datalen{0};
    bool is_file;
    bool is_buffer;
    bool has_filename;
};

class Multipart {
  public:
    Multipart(const std::initializer_list<Part>& parts);

    std::vector<Part> parts;
};

} // namespace cpr

#endif
