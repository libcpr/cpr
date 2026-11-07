#ifndef CPR_BUFFER_H
#define CPR_BUFFER_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <cstddef>
#endif

#include "cpr/filesystem.h"

namespace cpr {

EXPORT_CPR struct Buffer {
    using data_t = const char*;

    template <typename Iterator>
    Buffer(Iterator begin, Iterator end, fs::path&& p_filename)
            // Ignored here since libcurl requires a long.
            // There is also no way around the reinterpret_cast.
            // NOLINTNEXTLINE(google-runtime-int, cppcoreguidelines-pro-type-reinterpret-cast)
            : data{reinterpret_cast<data_t>(&(*begin))}, datalen{static_cast<size_t>(std::distance(begin, end))}, filename(std::move(p_filename)) {
        is_random_access_iterator(begin, end);
        static_assert(sizeof(*begin) == 1, "Only byte buffers can be used");
    }

    template <typename Iterator>
    static std::enable_if_t<std::is_same_v<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>> is_random_access_iterator(Iterator /* begin */, Iterator /* end */) {}

    data_t data;
    size_t datalen;
    const fs::path filename;
};

} // namespace cpr

#endif
