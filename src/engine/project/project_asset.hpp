#pragma once

struct ProjectAsset
{
    std::string Name{};
    std::vector<std::string> Scenes{};
    std::vector<int> Resolution{};
    float TargetFps{60.0f};
};
