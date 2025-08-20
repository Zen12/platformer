#pragma once

#include <random>
#include <sstream>
#include <iomanip>
#include <array>
#include <string>


class GuidGenerator{
public:
    GuidGenerator() = delete;

    static std::string GenerateGuid()
    {
        static thread_local std::mt19937_64 rng{std::random_device{}()};
        static thread_local std::uniform_int_distribution<uint64_t> dist;

    std::array<uint8_t, 16> bytes;
    for (auto& b : bytes) {
        b = static_cast<uint8_t>(dist(rng));
    }

    // Set version to 4 (random) and variant to 2 (RFC 4122)
    bytes[6] = (bytes[6] & 0x0F) | 0x40;
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < bytes.size(); i++) {
        oss << std::setw(2) << static_cast<int>(bytes[i]);
        if (i == 3 || i == 5 || i == 7 || i == 9)
            oss << "-";
    }
    return oss.str();
    }
};