#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Particle {
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Velocity = glm::vec3(0.0f);
    glm::vec4 Color = glm::vec4(1.0f);
    float Lifetime = 0.0f;
    float MaxLifetime = 1.0f;
    bool Active = false;
};
