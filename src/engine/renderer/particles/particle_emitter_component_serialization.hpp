#pragma once
#include "../../asset/serialization/serialization_component.hpp"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct ParticleEmitterSerialization final : public ComponentSerialization {
    float duration;
    float startScale;
    float endScale;
    size_t count;
    glm::vec3 startVelocity;
    glm::vec3 endVelocity;
    glm::vec4 startColor;
    glm::vec4 endColor;
    std::string materialGuid;
};