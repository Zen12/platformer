#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "guid.hpp"

class SimpleAnimationComponent final {
public:
    Guid AnimationGuid;
    float Time = 0.0f;
    bool Enabled = true;
    bool Loop = true;

    SimpleAnimationComponent() = default;
    explicit SimpleAnimationComponent(const Guid& animGuid)
        : AnimationGuid(animGuid) {}
};
