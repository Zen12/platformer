#pragma once
#include <string>
#include <vector>

struct ProjectConfig {
    std::string Name;
    std::vector<int> Resolution;
    float TargetFps{60.0f};
};
