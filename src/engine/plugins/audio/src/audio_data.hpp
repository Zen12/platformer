#pragma once
#include <vector>
#include <cstdint>

struct AudioData {
    std::vector<uint8_t> Data;
    uint32_t SampleRate = 0;
    uint16_t Channels = 0;
    uint16_t BitsPerSample = 0;

    [[nodiscard]] bool IsValid() const noexcept {
        return !Data.empty() && SampleRate > 0 && Channels > 0 && BitsPerSample > 0;
    }
};
