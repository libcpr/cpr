#ifndef CPR_RANGE_H
#define CPR_RANGE_H

#include <cstdint>

namespace cpr {

class Range {
  public:
    Range(const std::int64_t p_resume_from, const std::int64_t p_finish_at) : resume_from(p_resume_from), finish_at(p_finish_at) {}

    std::int64_t resume_from = 0;
    std::int64_t finish_at = 0;

    const std::string str() const {
        std::string from_str = (resume_from < (std::int64_t) 0) ? "" : std::to_string(resume_from);
        std::string to_str = (finish_at < (std::int64_t) 0) ? "" : std::to_string(finish_at);
        return from_str + "-" + to_str;
    }
};

class MultiRange {
  public:
    MultiRange(std::initializer_list<Range> rs) : ranges{rs} {}

    const std::string str() const {
        std::string multi_range_string{};
        for (Range range : ranges) {
            multi_range_string += ((multi_range_string.empty()) ? "" : ", ") + range.str();
        }
        return multi_range_string;
    }

  private:
    std::vector<Range> ranges;
}; // namespace cpr

} // namespace cpr

#endif
