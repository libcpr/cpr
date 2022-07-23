#ifndef CPR_FILE_H
#define CPR_FILE_H

#include <string>

#include <cpr/filesystem.h>

namespace cpr {

struct File {
    explicit File(fs::path&& p_filepath) : filepath(std::move(p_filepath)) {}
    explicit File(const fs::path& p_filepath) : filepath(p_filepath) {}
    const fs::path filepath;
};

} // namespace cpr

#endif
