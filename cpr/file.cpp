#include "cpr/file.h"

namespace cpr {

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
} // namespace cpr
