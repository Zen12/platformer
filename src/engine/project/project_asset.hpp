#pragma once

#include <string>
#include <vector>
#include "guid.hpp"

struct ProjectAsset
{
    std::string Name{};
    Guid MainFsm{};
    std::vector<int> Resolution{};
    float TargetFps{60.0f};
};
