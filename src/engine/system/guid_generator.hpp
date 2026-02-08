#pragma once

#include <random>
#include <array>
#include "guid.hpp"


class GuidGenerator {
public:
    GuidGenerator() = delete;

    static Guid GenerateGuid()
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

        uint64_t high = 0;
        uint64_t low = 0;

        for (int i = 0; i < 8; ++i) {
            high = (high << 8) | bytes[i];
        }
        for (int i = 8; i < 16; ++i) {
            low = (low << 8) | bytes[i];
        }

        return {high, low};
    }
};
