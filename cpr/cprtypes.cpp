#include "cprtypes.h"

#include <algorithm>


bool CaseInsenstiveCompare::operator()(const std::string& a, const std::string& b) const {
    return to_lower(a) < to_lower(b);
}

void CaseInsenstiveCompare::char_to_lower(char& c) {
    if (c >= 'A' && c <= 'Z')
    c += ('a' - 'A');
}

std::string CaseInsenstiveCompare::to_lower(const std::string& a) {
    std::string s(a);
    std::for_each(s.begin(), s.end(), char_to_lower);
    return s;
}
