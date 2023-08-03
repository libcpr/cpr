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
    Part(const std::string& p_name, const std::string& p_value, const std::string& p_content_type = {}) : name{p_name}, value{p_value}, content_type{p_content_type}, is_file{false}, is_buffer{false} {}
    Part(const std::string& p_name, const std::int32_t& p_value, const std::string& p_content_type = {}) : name{p_name}, value{std::to_string(p_value)}, content_type{p_content_type}, is_file{false}, is_buffer{false} {}
    Part(const std::string& p_name, const Files& p_files, const std::string& p_content_type = {}) : name{p_name}, content_type{p_content_type}, is_file{true}, is_buffer{false}, files{p_files} {}
    Part(const std::string& p_name, Files&& p_files, const std::string& p_content_type = {}) : name{p_name}, content_type{p_content_type}, is_file{true}, is_buffer{false}, files{p_files} {}
    Part(const std::string& p_name, const Buffer& buffer, const std::string& p_content_type = {}) : name{p_name}, value{buffer.filename.string()}, content_type{p_content_type}, data{buffer.data}, datalen{buffer.datalen}, is_file{false}, is_buffer{true} {}

    std::string name;
    // We don't use fs::path here, as this leads to problems using windows
    std::string value;
    std::string content_type;
    Buffer::data_t data{nullptr};
    size_t datalen{0};
    bool is_file;
    bool is_buffer;

    Files files;
};

class Multipart {
  public:
    Multipart(const std::initializer_list<Part>& parts);
    Multipart(const std::vector<Part>& parts);
    Multipart(const std::vector<Part>&& parts);

    std::vector<Part> parts;
};

} // namespace cpr

#endif
