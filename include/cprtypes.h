#ifndef CPR_TYPES_H
#define CPR_TYPES_H

#include <map>
#include <string>


class CaseInsenstiveCompare {
  public:
    bool operator()(const std::string& a, const std::string& b) const;

  private:
    static void char_to_lower(char& c);
    static std::string to_lower(const std::string& a);
};

typedef std::map<std::string, std::string> Parameters;
typedef std::map<std::string, std::string, CaseInsenstiveCompare> Header;
typedef std::string Url;
typedef long Timeout;

#endif
