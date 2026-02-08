#pragma once

#include <cstdint>
#include <string>
#include <functional>

class Guid {
    uint64_t _high = 0;
    uint64_t _low = 0;

public:
    constexpr Guid() noexcept = default;

    constexpr Guid(uint64_t high, uint64_t low) noexcept
        : _high(high), _low(low) {}

    [[nodiscard]] constexpr uint64_t High() const noexcept { return _high; }
    [[nodiscard]] constexpr uint64_t Low() const noexcept { return _low; }

    [[nodiscard]] constexpr bool IsEmpty() const noexcept {
        return _high == 0 && _low == 0;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return !IsEmpty();
    }

    [[nodiscard]] static Guid Generate();

    [[nodiscard]] static Guid FromString(const std::string& str);

    [[nodiscard]] std::string ToString() const;

    [[nodiscard]] constexpr bool operator==(const Guid& other) const noexcept {
        return _high == other._high && _low == other._low;
    }

    [[nodiscard]] constexpr bool operator!=(const Guid& other) const noexcept {
        return !(*this == other);
    }

    [[nodiscard]] constexpr bool operator<(const Guid& other) const noexcept {
        return _high < other._high || (_high == other._high && _low < other._low);
    }

    [[nodiscard]] constexpr bool operator>(const Guid& other) const noexcept {
        return other < *this;
    }

    [[nodiscard]] constexpr bool operator<=(const Guid& other) const noexcept {
        return !(other < *this);
    }

    [[nodiscard]] constexpr bool operator>=(const Guid& other) const noexcept {
        return !(*this < other);
    }
};

namespace std {
    template <>
    struct hash<Guid> {
        constexpr std::size_t operator()(const Guid& guid) const noexcept {
            constexpr std::size_t prime = 0x9e3779b97f4a7c15ULL;
            return (guid.High() * prime) ^ (guid.Low() * (prime >> 1));
        }
    };
}
