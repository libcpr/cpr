#ifndef CPR_FILESYSTEM_H
#define CPR_FILESYSTEM_H

// Include filesystem into the namespace "fs" from either "filesystem" or "experimental/filesystem"
#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include("experimental/filesystem")
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#error "Failed to include <filesystem> header!"
#endif

#endif