#include "multipart.h"

namespace cpr {

Multipart::Multipart(const std::initializer_list<Part>& parts) {
    for (auto part = parts.begin(); part != parts.end(); ++part) {
        this->parts.push_back(*part);
    }
}

}
