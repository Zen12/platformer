#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "../../entity/component_serialization.hpp"
#include "guid.hpp"

struct ParticleEmitterComponentSerialization final : public ComponentSerialization {
    Guid MaterialGuid;
    Guid MeshGuid;
    int MaxParticles = 100;
    float EmissionRate = 10.0f;
    float ParticleLifetime = 2.0f;
    float LifetimeVariance = 0.5f;
    glm::vec3 PositionOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 InitialVelocity = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 VelocityVariance = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 Gravity = glm::vec3(0.0f, -9.8f, 0.0f);
    glm::vec4 StartColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 EndColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    float ParticleSize = 0.1f;
    bool TriggerOnLand = false;
    bool TriggerOnDamage = false;
    bool TriggerOnAttack = false;
    int BurstCount = 10;
};
