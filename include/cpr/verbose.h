#ifndef CPR_VERBOSE_H_
#define CPR_VERBOSE_H_

#include <cstdbool>

namespace cpr {

class Verbose {
  public:
    Verbose() {}
    Verbose(const bool verbose) : verbose{verbose} {}

    bool verbose = true;
};

} // namespace cpr


#endif /* CPR_VERBOSE_H_ */