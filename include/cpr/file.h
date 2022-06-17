#ifndef CPR_FILE_H
#define CPR_FILE_H

#include <string>

namespace cpr {

struct File {
    explicit File(std::string&& p_filepath) : filepath(std::move(p_filepath)) {}
    explicit File(const std::string& p_filepath) : filepath(p_filepath) {}
    const std::string filepath;
};

} // namespace cpr

#endif
