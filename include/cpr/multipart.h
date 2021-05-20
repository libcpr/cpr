#ifndef CPR_MULTIPART_H
#define CPR_MULTIPART_H

#include <cstdint>
#include <initializer_list>
#include <string>
#include <type_traits>
#include <vector>

namespace cpr {

struct File {
    explicit File(std::string&& p_filepath) : filepath(std::move(p_filepath)) {}
    explicit File(const std::string& p_filepath) : filepath(p_filepath) {}
    const std::string filepath;
};

struct Buffer {
    using data_t = const unsigned char*;

    template <typename Iterator>
    Buffer(Iterator begin, Iterator end, std::string&& p_filename)
            // Ignored here since libcurl reqires a long.
            // There is also no way around the reinterpret_cast.
            // NOLINTNEXTLINE(google-runtime-int, cppcoreguidelines-pro-type-reinterpret-cast)
            : data{reinterpret_cast<data_t>(&(*begin))}, datalen{static_cast<long>(
                                                            std::distance(begin, end))},
              filename(std::move(p_filename)) {
        is_random_access_iterator(begin, end);
        static_assert(sizeof(*begin) == 1, "only byte buffers can be used");
    }

    template <typename Iterator>
    typename std::enable_if<std::is_same<typename std::iterator_traits<Iterator>::iterator_category,
                                         std::random_access_iterator_tag>::value>::type
    is_random_access_iterator(Iterator /* begin */, Iterator /* end */) {}

    data_t data;
    // Ignored here since libcurl reqires a long:
    // NOLINTNEXTLINE(google-runtime-int)
    long datalen;
    const std::string filename;
};

struct Part {
    Part(const std::string& p_name, const std::string& p_value, const std::string& p_content_type = {})
            : name{p_name}, value{p_value},
              content_type{p_content_type}, is_file{false}, is_buffer{false} {}
    Part(const std::string& p_name, const std::int32_t& p_value, const std::string& p_content_type = {})
            : name{p_name}, value{std::to_string(p_value)},
              content_type{p_content_type}, is_file{false}, is_buffer{false} {}
    Part(const std::string& p_name, const File& file, const std::string& p_content_type = {})
            : name{p_name}, value{file.filepath},
              content_type{p_content_type}, is_file{true}, is_buffer{false} {}
    Part(const std::string& p_name, const Buffer& buffer, const std::string& p_content_type = {})
            : name{p_name}, value{buffer.filename}, content_type{p_content_type}, data{buffer.data},
              datalen{buffer.datalen}, is_file{false}, is_buffer{true} {}

    std::string name;
    std::string value;
    std::string content_type;
    Buffer::data_t data{nullptr};
    // Ignored here since libcurl reqires a long:
    // NOLINTNEXTLINE(google-runtime-int)
    long datalen{0};
    bool is_file;
    bool is_buffer;
};

class Multipart {
  public:
    Multipart(const std::initializer_list<Part>& parts);

    std::vector<Part> parts;
};

} // namespace cpr

#endif
