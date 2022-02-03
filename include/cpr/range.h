#ifndef CPR_RANGE_H
#define CPR_RANGE_H

#include <cstdint>

namespace cpr {

class Range {
  public:
    Range(const std::int64_t p_resume_from, const std::int64_t p_finish_at)
            : resume_from(p_resume_from), finish_at(p_finish_at) {}

    std::int64_t resume_from = 0;
    std::int64_t finish_at = 0;
};

} // namespace cpr

#endif
