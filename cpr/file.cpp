#include "cpr/file.h"
#include <initializer_list>
#include <string>
#include <utility>

namespace cpr {

Files::Files(const std::initializer_list<std::string>& p_filepaths) : files(p_filepaths.begin(), p_filepaths.end()) {}

Files::iterator Files::begin() {
    return files.begin();
}

Files::iterator Files::end() {
    return files.end();
}

Files::const_iterator Files::begin() const {
    return files.begin();
}

Files::const_iterator Files::end() const {
    return files.end();
}

Files::const_iterator Files::cbegin() const {
    return files.cbegin();
}

Files::const_iterator Files::cend() const {
    return files.cend();
}

void Files::emplace_back(const File& file) {
    files.emplace_back(file);
}

void Files::push_back(const File& file) {
    files.push_back(file);
}

void Files::pop_back() {
    files.pop_back();
}

Files& Files::operator=(const Files& other) {
    if (&other != this) {
        files = other.files;
    }
    return *this;
}

Files& Files::operator=(Files&& old) noexcept {
    if (&old != this) {
        files = std::move(old.files);
    }
    return *this;
}
} // namespace cpr
