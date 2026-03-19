#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "guid/guid.hpp"

struct MeshGenOperation {
    std::string Type;

    glm::vec3 Size{1.0f};
    float Radius{1.0f};
    float Height{1.0f};
    int Segments{16};

    glm::vec3 Translate{0.0f};
    glm::vec3 Rotate{0.0f};
    glm::vec3 Scale{1.0f};

    glm::vec3 Color{1.0f, 1.0f, 1.0f};

    Guid BezierGuid;
    Guid MeshGuid;
    std::vector<int> Operands;
};

struct MeshGenAsset {
    std::string Name;
    std::vector<MeshGenOperation> Operations;
};
