#include "cpr/redirect.h"
#include <type_traits>

namespace cpr {
PostRedirectFlags operator|(PostRedirectFlags lhs, PostRedirectFlags rhs) {
    return static_cast<PostRedirectFlags>(static_cast<std::underlying_type<PostRedirectFlags>::type>(lhs) | static_cast<std::underlying_type<PostRedirectFlags>::type>(rhs));
}

PostRedirectFlags operator&(PostRedirectFlags lhs, PostRedirectFlags rhs) {
    return static_cast<PostRedirectFlags>(static_cast<std::underlying_type<PostRedirectFlags>::type>(lhs) & static_cast<std::underlying_type<PostRedirectFlags>::type>(rhs));
}

PostRedirectFlags operator^(PostRedirectFlags lhs, PostRedirectFlags rhs) {
    return static_cast<PostRedirectFlags>(static_cast<std::underlying_type<PostRedirectFlags>::type>(lhs) ^ static_cast<std::underlying_type<PostRedirectFlags>::type>(rhs));
}

PostRedirectFlags operator~(PostRedirectFlags flag) {
    return static_cast<PostRedirectFlags>(~static_cast<std::underlying_type<PostRedirectFlags>::type>(flag));
}

PostRedirectFlags& operator|=(PostRedirectFlags& lhs, PostRedirectFlags rhs) {
    lhs = static_cast<PostRedirectFlags>(static_cast<std::underlying_type<PostRedirectFlags>::type>(lhs) | static_cast<std::underlying_type<PostRedirectFlags>::type>(rhs));
    uint8_t tmp = static_cast<uint8_t>(lhs);
    lhs = static_cast<PostRedirectFlags>(tmp);
    return lhs;
}

PostRedirectFlags& operator&=(PostRedirectFlags& lhs, PostRedirectFlags rhs) {
    lhs = static_cast<PostRedirectFlags>(static_cast<std::underlying_type<PostRedirectFlags>::type>(lhs) & static_cast<std::underlying_type<PostRedirectFlags>::type>(rhs));
    return lhs;
}

PostRedirectFlags& operator^=(PostRedirectFlags& lhs, PostRedirectFlags rhs) {
    lhs = static_cast<PostRedirectFlags>(static_cast<std::underlying_type<PostRedirectFlags>::type>(lhs) ^ static_cast<std::underlying_type<PostRedirectFlags>::type>(rhs));
    return lhs;
}

bool any(PostRedirectFlags flag) {
    return flag != PostRedirectFlags::NONE;
}
} // namespace cpr
