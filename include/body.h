#ifndef CPR_BODY_H
#define CPR_BODY_H

#include <string>

#include "defines.h"

namespace cpr {

class Body {
  public:
    template <typename TextType>
    Body(TextType&& p_text)
            : text{CPR_FWD(p_text)} {}

    std::string text;
};

} // namespace cpr

#endif
