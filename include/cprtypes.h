#ifndef CPR_TYPES_H
#define CPR_TYPES_H

#include <map>
#include <string>

namespace cpr {

    class CaseInsenstiveCompare {
      public:
        bool operator()(const std::string& a, const std::string& b) const;

      private:
        static void char_to_lower(char& c);
        static std::string to_lower(const std::string& a);
    };

    using Header = std::map<std::string, std::string, CaseInsenstiveCompare>;
    using Url = std::string;

} // namespace cpr

#endif
