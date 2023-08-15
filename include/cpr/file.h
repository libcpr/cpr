#ifndef CPR_FILE_H
#define CPR_FILE_H

#include <initializer_list>
#include <string>
#include <vector>

#include "cpr/filesystem.h"

namespace cpr {

struct File {
    explicit File(std::string p_filepath, const std::string& p_overriden_filename = {}) : filepath(std::move(p_filepath)), overriden_filename(p_overriden_filename) {}

    std::string filepath;
    std::string overriden_filename;

    [[nodiscard]] bool hasOverridenFilename() const noexcept {
        return !overriden_filename.empty();
    }
};

class Files {
  public:
    Files() = default;
    // NOLINTNEXTLINE(google-explicit-constructor)
    Files(const File& p_file) : files{p_file} {}

    Files(const Files& other) = default;
    Files(Files&& old) noexcept = default;

    Files(const std::initializer_list<File>& p_files) : files{p_files} {}
    Files(const std::initializer_list<std::string>& p_filepaths);

    ~Files() noexcept = default;

    Files& operator=(const Files& other);
    Files& operator=(Files&& old) noexcept;

    using iterator = std::vector<File>::iterator;
    using const_iterator = std::vector<File>::const_iterator;

    iterator begin();
    iterator end();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;
    [[nodiscard]] const_iterator cbegin() const;
    [[nodiscard]] const_iterator cend() const;
    void emplace_back(const File& file);
    void push_back(const File& file);
    void pop_back();

  private:
    std::vector<File> files;
};

} // namespace cpr

#endif
