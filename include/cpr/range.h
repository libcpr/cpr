#ifndef CPR_RANGE_H
#define CPR_RANGE_H

#include <cassert>
#include <cstdint>
#include <curl/curl.h>

#define assertm(exp, msg) assert(((void) msg, exp))

namespace cpr {

class Range {
  public:
    Range(const std::int64_t p_resume_from, const std::int64_t p_finish_at) : resume_from(p_resume_from), finish_at(p_finish_at) {}

    std::int64_t getResumeFrom() const {
        return resume_from;
    }

    std::int64_t getFinishAt() const {
        return finish_at;
    }

    const std::string str() const {
        std::string from_str = (resume_from < (std::int64_t) 0) ? "" : std::to_string(resume_from);
        std::string to_str = (finish_at < (std::int64_t) 0) ? "" : std::to_string(finish_at);
        return from_str + "-" + to_str;
    }

  private:
    std::int64_t resume_from = 0;
    std::int64_t finish_at = 0;
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
};

class UploadRange : private Range {
  public:
    UploadRange(const std::int64_t p_resume_from, const std::int64_t p_finish_at) : Range(p_resume_from, p_finish_at) {
        // Upload ranges do not support negative values / partial ranges
        assertm(p_resume_from >= 0, "UploadRange does not support negative values / partial ranges");
        assertm(p_finish_at >= 0, "UploadRange does not support negative values / partial ranges");
        assertm(p_resume_from <= p_finish_at, "p_resume_from has to be smaller equal than p_resume_from");
    }

    curl_off_t getResumeFrom() const {
        return static_cast<curl_off_t>(this->Range::getResumeFrom());
    }

    curl_off_t getFilesize() const {
        // More information: https://curl.se/mail/lib-2019-05/0012.html
        // filesize = resume_from + part size =
        //          = resume_from + (finish_at - resume_from + 1)
        //          = finish_at + 1
        // Note: We assume that the actual filesize is larger than or equal to finish_at
        return static_cast<curl_off_t>(this->Range::getFinishAt()) + 1;
    }
};

} // namespace cpr

#endif
