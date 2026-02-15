#ifndef CPR_VERBOSE_H_
#define CPR_VERBOSE_H_

namespace cpr {

class Verbose {
  public:
    Verbose() = default;
    Verbose(const bool p_verbose) : verbose{p_verbose} {}

    bool verbose = true;
};

} // namespace cpr


#endif /* CPR_VERBOSE_H_ */
