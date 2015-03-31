#include "auth.h"


const char* Authentication::GetAuthString() const {
    return std::string{username_ + ":" + password_}.data();
}
