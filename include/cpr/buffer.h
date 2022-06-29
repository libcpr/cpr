#ifndef CPR_BUFFER_H
#define CPR_BUFFER_H

#include <string>

namespace cpr {

struct Buffer {
    using data_t = const unsigned char*;

    template <typename Iterator>
    Buffer(Iterator begin, Iterator end, std::string&& p_filename)
            // Ignored here since libcurl reqires a long.
            // There is also no way around the reinterpret_cast.
            // NOLINTNEXTLINE(google-runtime-int, cppcoreguidelines-pro-type-reinterpret-cast)
            : data{reinterpret_cast<data_t>(&(*begin))}, datalen{static_cast<long>(std::distance(begin, end))}, filename(std::move(p_filename)) {
        is_random_access_iterator(begin, end);
        static_assert(sizeof(*begin) == 1, "Only byte buffers can be used");
    }

    template <typename Iterator>
    typename std::enable_if<std::is_same<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>::value>::type is_random_access_iterator(Iterator /* begin */, Iterator /* end */) {}

    data_t data;
    // Ignored here since libcurl reqires a long:
    // NOLINTNEXTLINE(google-runtime-int)
    long datalen;
    const std::string filename;
};

} // namespace cpr

#endif
