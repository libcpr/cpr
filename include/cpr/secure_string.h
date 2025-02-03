#ifndef CPR_SECURE_STRING_H
#define CPR_SECURE_STRING_H

#include <memory>
#include <string>
#include <string_view>

namespace cpr::util {

// This is an allocator that overwrites memory with zero values before
// deallocating the memory, so as to not leave secrets in unallocated memory
// sections.
template <typename T>
struct SecureAllocator : private std::allocator<T> {
    template <typename U>
    friend struct SecureAllocator;
    SecureAllocator() = default;
    template <typename U>
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    SecureAllocator(const SecureAllocator<U>& rhs) noexcept : std::allocator<T>(static_cast<const std::allocator<U>&>(rhs)) {}
    template <typename U>
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    SecureAllocator(SecureAllocator<U>&& rhs) noexcept : std::allocator<T>(static_cast<std::allocator<U>&&>(rhs)) {}
    template <typename U>
    SecureAllocator& operator=(const SecureAllocator<U>& rhs) noexcept {
        static_cast<std::allocator<T>&>(*this) = static_cast<const std::allocator<U>&>(rhs);
        return *this;
    }
    template <typename U>
    SecureAllocator& operator=(SecureAllocator<U>&& rhs) noexcept {
        static_cast<std::allocator<T>&>(*this) = static_cast<std::allocator<U>&&>(rhs);
        return *this;
    }

    using value_type = T;

    // NOLINTNEXTLINE(readability-identifier-naming)
    T* allocate(std::size_t n) {
        return static_cast<std::allocator<T>&>(*this).allocate(n);
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    void deallocate(T* p, std::size_t n) {
        std::fill_n(p, n, T{});
        static_cast<std::allocator<T>&>(*this).deallocate(p, n);
    }

    template <typename U>
    [[nodiscard]] bool IsEqual(const SecureAllocator<U>& rhs) const noexcept {
        return static_cast<const std::allocator<T>&>(*this) == static_cast<const std::allocator<U>&>(rhs);
    }
};
template <typename T, typename U>
bool operator==(const SecureAllocator<T>& lhs, const SecureAllocator<U>& rhs) noexcept {
    return lhs.IsEqual(rhs);
}
template <typename T, typename U>
bool operator!=(const SecureAllocator<T>& lhs, const SecureAllocator<U>& rhs) noexcept {
    return !lhs.IsEqual(rhs);
}

using SecureString = std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>;

} // namespace cpr::util

#endif // CPR_SECURE_STRING_H
