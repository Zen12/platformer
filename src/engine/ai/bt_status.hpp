#pragma once
#include <cstdint>

enum class BTStatus : uint8_t {
    Success = 0,
    Failure = 1,
    Running = 2,
};
