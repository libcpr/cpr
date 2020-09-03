#include "cpr/curl_container.h"


namespace cpr {


template<class T>
CurlContainer<T>::CurlContainer(const std::initializer_list<T>& containerList) : containerList_(containerList) {

}


template<class T>
void CurlContainer<T>::Add(const std::initializer_list<T>& containerList) {
    for(const auto& element : containerList) {
        containerList_.push_back(std::move(element));
    }
}

template<class T>
void CurlContainer<T>::Add(const T& element) {
    containerList_.push_back(std::move(element));
}

template<>
const std::string CurlContainer<Parameter>::GetContent(const CurlHolder& holder) const {
    std::string content{};
    for(const auto& parameter : containerList_) {
        if (!content.empty()) {
            content += "&";
        }

        std::string escapedKey = holder.urlEncode(parameter.key);
        if (parameter.value.empty()) {
            content += escapedKey;
        } else {
            std::string escapedValue = holder.urlEncode(parameter.value);
            content += escapedKey + "=" + escapedValue;
        }
    };

    return content;
}

template<>
const std::string CurlContainer<Pair>::GetContent(const CurlHolder& holder) const {
    std::string content{};
    for(const auto& element : containerList_) {
        if (!content.empty()) {
            content += "&";
        }
        std::string escaped = holder.urlEncode(element.value);
        content += element.key + "=" + escaped;
    }

    return content;
}

template class CurlContainer<Pair>;
template class CurlContainer<Parameter>;

} // namespace cpr
