#include "guid.hpp"
#include <sstream>
#include <iomanip>
#include <random>

Guid Guid::Generate() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dist;

    uint64_t high = dist(gen);
    uint64_t low = dist(gen);

    // Set version to 4 (random UUID) in bits 12-15 of time_hi_and_version
    high = (high & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    // Set variant to RFC 4122 (10xx) in bits 62-63 of clock_seq_hi_and_reserved
    low = (low & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;

    return {high, low};
}

Guid Guid::FromString(const std::string& str) {
    if (str.empty()) {
        return {};
    }

    std::string cleaned;
    cleaned.reserve(32);
    for (const char c : str) {
        if (c != '-') {
            cleaned += c;
        }
    }

    if (cleaned.size() != 32) {
        return {};
    }

    const std::string highStr = cleaned.substr(0, 16);
    const std::string lowStr = cleaned.substr(16, 16);

    return {std::stoull(highStr, nullptr, 16), std::stoull(lowStr, nullptr, 16)};
}

std::string Guid::ToString() const {
    if (IsEmpty()) {
        return {};
    }

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    oss << std::setw(8) << ((_high >> 32) & 0xFFFFFFFF);
    oss << '-';
    oss << std::setw(4) << ((_high >> 16) & 0xFFFF);
    oss << '-';
    oss << std::setw(4) << (_high & 0xFFFF);
    oss << '-';
    oss << std::setw(4) << ((_low >> 48) & 0xFFFF);
    oss << '-';
    oss << std::setw(12) << (_low & 0xFFFFFFFFFFFF);

    return oss.str();
}
