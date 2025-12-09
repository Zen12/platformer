#pragma once

#include <string>

struct ProjectAsset
{
    std::string Name{};
    std::string MainFsm{};
    std::vector<int> Resolution{};
    float TargetFps{60.0f};
};
