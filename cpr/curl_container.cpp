#include "cpr/curl_container.h"
#include <algorithm>
#include <iterator>


namespace cpr {
template <class T>
CurlContainer<T>::CurlContainer(const std::initializer_list<T>& containerList) : containerList_(containerList) {}

template <class T>
void CurlContainer<T>::Add(const std::initializer_list<T>& containerList) {
    std::transform(containerList.begin(), containerList.end(), std::back_inserter(containerList_), [](const T& elem) { return std::move(elem); });
}

template <class T>
void CurlContainer<T>::Add(const T& element) {
    containerList_.push_back(std::move(element));
}

template <>
const std::string CurlContainer<Parameter>::GetContent(const CurlHolder& holder) const {
    std::string content{};
    for (const Parameter& parameter : containerList_) {
        if (!content.empty()) {
            content += "&";
        }

        std::string escapedKey = encode ? holder.urlEncode(parameter.key) : parameter.key;
        if (parameter.value.empty()) {
            content += escapedKey;
        } else {
            std::string escapedValue = encode ? holder.urlEncode(parameter.value) : parameter.value;
            content += escapedKey + "=";
            content += escapedValue;
        }
    };

    return content;
}

template <>
const std::string CurlContainer<Pair>::GetContent(const CurlHolder& holder) const {
    std::string content{};
    for (const cpr::Pair& element : containerList_) {
        if (!content.empty()) {
            content += "&";
        }
        std::string escaped = encode ? holder.urlEncode(element.value) : element.value;
        content += element.key + "=" + escaped;
    }

    return content;
}

template class CurlContainer<Pair>;
template class CurlContainer<Parameter>;

} // namespace cpr
