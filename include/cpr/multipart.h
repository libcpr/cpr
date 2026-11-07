#ifndef CPR_MULTIPART_H
#define CPR_MULTIPART_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#endif

#include "buffer.h"
#include "callback.h"
#include "file.h"

namespace cpr {

/** A callback-backed multipart body, read incrementally without buffering it.
 * ReadCallback::size is the byte count, or -1 if unknown. Set the callback's
 * size argument to the number of bytes read (zero at EOF), and return false
 * to abort. Captured resources must remain alive until the request completes.
 * An optional seek function takes a byte offset and a SEEK_* origin and returns
 * true on success. Without it, resending the part may fail, e.g. on redirects.
 */
EXPORT_CPR struct MultipartStream {
    using SeekFunction = std::function<bool(cpr_off_t offset, int origin)>;

    explicit MultipartStream(ReadCallback p_read, std::string p_filename = {}, SeekFunction p_seek = {}) : read{std::move(p_read)}, filename{std::move(p_filename)}, seek{std::move(p_seek)} {
        if (!read.callback || read.size < -1) {
            throw std::invalid_argument("MultipartStream requires a read callback and a size of -1 or greater");
        }
    }

    ReadCallback read;
    std::string filename;
    SeekFunction seek;
};

EXPORT_CPR struct Part {
    Part(const std::string& p_name, const std::string& p_value, const std::string& p_content_type = {}) : name{p_name}, value{p_value}, content_type{p_content_type}, is_file{false}, is_buffer{false} {}
    Part(const std::string& p_name, const std::int32_t& p_value, const std::string& p_content_type = {}) : name{p_name}, value{std::to_string(p_value)}, content_type{p_content_type}, is_file{false}, is_buffer{false} {}
    Part(const std::string& p_name, const Files& p_files, const std::string& p_content_type = {}) : name{p_name}, content_type{p_content_type}, is_file{true}, is_buffer{false}, files{p_files} {}
    Part(const std::string& p_name, Files&& p_files, const std::string& p_content_type = {}) : name{p_name}, content_type{p_content_type}, is_file{true}, is_buffer{false}, files{std::move(p_files)} {}
    Part(const std::string& p_name, const Buffer& buffer, const std::string& p_content_type = {}) : name{p_name}, value{buffer.filename.string()}, content_type{p_content_type}, data{buffer.data}, datalen{buffer.datalen}, is_file{false}, is_buffer{true} {}
    Part(const std::string& p_name, MultipartStream p_stream, const std::string& p_content_type = {}) : name{p_name}, content_type{p_content_type}, is_file{false}, is_buffer{false}, stream{std::move(p_stream)} {}

    std::string name;
    // We don't use fs::path here, as this leads to problems using windows
    std::string value;
    std::string content_type;
    Buffer::data_t data{nullptr};
    size_t datalen{0};
    bool is_file;
    bool is_buffer;

    Files files;
    std::optional<MultipartStream> stream;
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
