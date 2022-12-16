#ifndef CPR_FILE_H
#define CPR_FILE_H

#include <initializer_list>
#include <string>
#include <vector>

#include <cpr/filesystem.h>

namespace cpr {

struct File {
    explicit File(std::string p_filepath, const std::string& p_overrided_filename = {}) : filepath(std::move(p_filepath)), overrided_filename(p_overrided_filename) {}

    const std::string filepath;
    const std::string overrided_filename;

    bool hasOverridedFilename() const noexcept {
        return !overrided_filename.empty();
    };
};

class Files {
  public:
    Files() = default;
    Files(const File& p_file) : files{p_file} {};
    Files(const std::initializer_list<File>& p_files) : files{p_files} {};
    Files(const std::initializer_list<std::string>& p_filepaths) {
        for (const std::string& filepath : p_filepaths) {
            files.emplace_back(File(filepath));
        }
    };
    ~Files() noexcept = default;

    using iterator = std::vector<File>::iterator;
    using const_iterator = std::vector<File>::const_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    void emplace_back(const File& file);
    void push_back(const File& file);
    void pop_back();

  private:
    std::vector<File> files;
};

} // namespace cpr

#endif
