#ifndef CPR_FILESYSTEM_H
#define CPR_FILESYSTEM_H

// Include filesystem into the namespace "fs" from either "filesystem" or "experimental/filesystem" or "boost/filesystem"
#ifdef CPR_USE_BOOST_FILESYSTEM
#define BOOST_FILESYSTEM_VERSION 4 // Use the latest, with the closest behavior to std::filesystem.
#include <boost/filesystem.hpp>
namespace cpr {
namespace fs = boost::filesystem;
}
#elif defined(CPR_IMPORT_STD)
namespace cpr {
namespace fs = std::filesystem;
} // namespace cpr
// cppcheck-suppress preprocessorErrorDirective
#elif __has_include(<filesystem>)
/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <filesystem>
#endif
namespace cpr {
namespace fs = std::filesystem;
} // namespace cpr
#elif __has_include("experimental/filesystem")
#ifndef CPR_IMPORT_STD
#include <experimental/filesystem>
#endif
namespace cpr {
namespace fs = std::experimental::filesystem;
}
#else
#error "Failed to include <filesystem> header!"
#endif

#endif
