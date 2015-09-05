#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <memory>
#include <string>
#include <initializer_list>

namespace cpr {

    struct Parameter {
        Parameter(const std::string& key, const std::string& value) : key{key}, value{value} {}

        std::string key;
        std::string value;
    };

    class Parameters {
      public:
        Parameters() {}
        Parameters(const std::initializer_list<Parameter>& parameters);

        std::string content;
    };

}

#endif
