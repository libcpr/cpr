#ifndef CPR_PARAMETERS_H
#define CPR_PARAMETERS_H

#include <memory>
#include <string>
#include <initializer_list>
#include <list>

#include "defines.h"

namespace cpr {

    struct Parameter {
        template <typename KeyType, typename ValueType>
        Parameter(KeyType&& key, ValueType&& value)
                : key{CPR_FWD(key)}, value{CPR_FWD(value)} {}

        std::string key;
        std::string value;
    };

    class Parameters {
    public:
        Parameters() = default;
        Parameters(const std::initializer_list<Parameter>& parameters);

        //Update lib constructor that take a list of tuple
        typedef std::pair<std::string, std::string> type_parameter;
        typedef std::list<type_parameter> type_parameterS; // parameters are a list of tuple
        Parameters(const type_parameterS &parameters);


        void AddParameter(const Parameter& parameter);

        std::string content;
    };

} // namespace cpr

#endif