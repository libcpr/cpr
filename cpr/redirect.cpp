#include "cpr/redirect.h"

namespace cpr {
PostRedirectFlags operator|(PostRedirectFlags lhs, PostRedirectFlags rhs) {
    return static_cast<PostRedirectFlags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

PostRedirectFlags operator&(PostRedirectFlags lhs, PostRedirectFlags rhs) {
    return static_cast<PostRedirectFlags>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

PostRedirectFlags operator^(PostRedirectFlags lhs, PostRedirectFlags rhs) {
    return static_cast<PostRedirectFlags>(static_cast<uint8_t>(lhs) ^ static_cast<uint8_t>(rhs));
}

PostRedirectFlags operator~(PostRedirectFlags flag) {
    return static_cast<PostRedirectFlags>(~static_cast<uint8_t>(flag));
}

PostRedirectFlags& operator|=(PostRedirectFlags& lhs, PostRedirectFlags rhs) {
    lhs = static_cast<PostRedirectFlags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
    uint8_t tmp = static_cast<uint8_t>(lhs);
    lhs = static_cast<PostRedirectFlags>(tmp);
    return lhs;
}

PostRedirectFlags& operator&=(PostRedirectFlags& lhs, PostRedirectFlags rhs) {
    lhs = static_cast<PostRedirectFlags>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
    return lhs;
}

PostRedirectFlags& operator^=(PostRedirectFlags& lhs, PostRedirectFlags rhs) {
    lhs = static_cast<PostRedirectFlags>(static_cast<uint8_t>(lhs) ^ static_cast<uint8_t>(rhs));
    return lhs;
}

bool any(PostRedirectFlags flag) {
    return flag != PostRedirectFlags::NONE;
}
} // namespace cpr
