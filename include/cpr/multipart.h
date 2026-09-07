#ifndef CPR_MULTIPART_H
#define CPR_MULTIPART_H

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

#include "buffer.h"
#include "file.h"
#include <cstdint>

namespace cpr {

EXPORT_CPR struct Part {
    Part(const std::string& p_name, const std::string& p_value, const std::string& p_content_type = {}) : name{p_name}, value{p_value}, content_type{p_content_type}, is_file{false}, is_buffer{false} {}
    Part(const std::string& p_name, const std::int32_t& p_value, const std::string& p_content_type = {}) : name{p_name}, value{std::to_string(p_value)}, content_type{p_content_type}, is_file{false}, is_buffer{false} {}
    Part(const std::string& p_name, const Files& p_files, const std::string& p_content_type = {}) : name{p_name}, content_type{p_content_type}, is_file{true}, is_buffer{false}, files{p_files} {}
    Part(const std::string& p_name, Files&& p_files, const std::string& p_content_type = {}) : name{p_name}, content_type{p_content_type}, is_file{true}, is_buffer{false}, files{std::move(p_files)} {}
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

EXPORT_CPR class Multipart {
  public:
    Multipart(const std::initializer_list<Part>& parts);
    explicit Multipart(const std::vector<Part>& parts);
    explicit Multipart(const std::vector<Part>&& parts);

    std::vector<Part> parts;
};

} // namespace cpr

#endif
