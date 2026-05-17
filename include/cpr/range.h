#ifndef CPR_RANGE_H
#define CPR_RANGE_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <optional>
#include <string>
#include <vector>
#endif

#include <cstdint>

namespace cpr {

EXPORT_CPR class Range {
  public:
    explicit Range(const std::optional<std::int64_t> p_resume_from = std::nullopt, const std::optional<std::int64_t> p_finish_at = std::nullopt) {
        resume_from = p_resume_from.value_or(0);
        finish_at = p_finish_at.value_or(-1);
    }

    std::int64_t resume_from;
    std::int64_t finish_at;

    [[nodiscard]] const std::string str() const {
        std::string const from_str = (resume_from < 0U) ? "" : std::to_string(resume_from);
        std::string const to_str = (finish_at < 0U) ? "" : std::to_string(finish_at);
        return from_str + "-" + to_str;
    }
};

EXPORT_CPR class MultiRange {
  public:
    MultiRange(std::initializer_list<Range> rs) : ranges{rs} {}

    [[nodiscard]] const std::string str() const {
        std::string multi_range_string{};
        for (Range const range : ranges) {
            multi_range_string += ((multi_range_string.empty()) ? "" : ", ") + range.str();
        }
        return multi_range_string;
    }

  private:
    std::vector<Range> ranges;
}; // namespace cpr

} // namespace cpr

#endif
