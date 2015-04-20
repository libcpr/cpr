#include "digest.h"


const char* Digest::GetAuthString() const {
    return Authentication::GetAuthString();
}
