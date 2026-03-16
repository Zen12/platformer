#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct BezierAsset {
    std::string Name;
    std::vector<glm::vec3> ControlPoints;
};
