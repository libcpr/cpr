#include "auth.h"


const char* Authentication::GetAuthString() const {
    return auth_string_.data();
}
