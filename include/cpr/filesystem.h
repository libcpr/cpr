#ifndef CPR_FILESYSTEM_H
#define CPR_FILESYSTEM_H

// Include filesystem into the namespace "fs" from either "filesystem" or "experimental/filesystem" or "boost/filesystem"
#ifdef CPR_USE_BOOST_FILESYSTEM
#define BOOST_FILESYSTEM_VERSION 4 // Use the latest, with the closest behavior to std::filesystem.
#include <boost/filesystem.hpp>
namespace cpr {
namespace fs = boost::filesystem;
}
// cppcheck-suppress preprocessorErrorDirective
#elif __has_include(<filesystem>)
#include <filesystem>
namespace cpr {
namespace fs = std::filesystem;
}
#elif __has_include("experimental/filesystem")
#include <experimental/filesystem>
namespace cpr {
namespace fs = std::experimental::filesystem;
}
#else
#error "Failed to include <filesystem> header!"
#endif

#endif
