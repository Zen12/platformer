#pragma once
#include <vector>
#include <random>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "particle.hpp"
#include "particle_emitter_component_serialization.hpp"
#include "../../system/guid.hpp"

class ParticleEmitterComponent final {
private:
    std::vector<Particle> _particles;
    Guid _materialGuid;
    Guid _meshGuid;
    int _maxParticles = 100;
    float _emissionRate = 10.0f;
    float _particleLifetime = 2.0f;
    float _lifetimeVariance = 0.5f;
    glm::vec3 _positionOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 _initialVelocity = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 _velocityVariance = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 _gravity = glm::vec3(0.0f, -9.8f, 0.0f);
    glm::vec4 _startColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 _endColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    float _particleSize = 0.1f;
    bool _triggerOnLand = false;
    bool _triggerOnDamage = false;
    bool _triggerOnAttack = false;
    int _burstCount = 10;

    float _emissionAccumulator = 0.0f;
    std::mt19937 _rng;
    int _pendingBurst = 0;

public:
    ParticleEmitterComponent() : _rng(std::random_device{}()) {
        _particles.resize(_maxParticles);
    }

    explicit ParticleEmitterComponent(const ParticleEmitterComponentSerialization& serialization)
        : _materialGuid(serialization.MaterialGuid),
          _meshGuid(serialization.MeshGuid),
          _maxParticles(serialization.MaxParticles),
          _emissionRate(serialization.EmissionRate),
          _particleLifetime(serialization.ParticleLifetime),
          _lifetimeVariance(serialization.LifetimeVariance),
          _positionOffset(serialization.PositionOffset),
          _initialVelocity(serialization.InitialVelocity),
          _velocityVariance(serialization.VelocityVariance),
          _gravity(serialization.Gravity),
          _startColor(serialization.StartColor),
          _endColor(serialization.EndColor),
          _particleSize(serialization.ParticleSize),
          _triggerOnLand(serialization.TriggerOnLand),
          _triggerOnDamage(serialization.TriggerOnDamage),
          _triggerOnAttack(serialization.TriggerOnAttack),
          _burstCount(serialization.BurstCount),
          _emissionAccumulator(0.0f),
          _rng(std::random_device{}())
    {
        _particles.resize(_maxParticles);
    }

    [[nodiscard]] std::vector<Particle>& GetParticles() noexcept { return _particles; }
    [[nodiscard]] const std::vector<Particle>& GetParticles() const noexcept { return _particles; }

    [[nodiscard]] const Guid& GetMaterialGuid() const noexcept { return _materialGuid; }
    [[nodiscard]] const Guid& GetMeshGuid() const noexcept { return _meshGuid; }
    [[nodiscard]] int GetMaxParticles() const noexcept { return _maxParticles; }
    [[nodiscard]] float GetEmissionRate() const noexcept { return _emissionRate; }
    [[nodiscard]] float GetParticleLifetime() const noexcept { return _particleLifetime; }
    [[nodiscard]] float GetLifetimeVariance() const noexcept { return _lifetimeVariance; }
    [[nodiscard]] const glm::vec3& GetPositionOffset() const noexcept { return _positionOffset; }
    [[nodiscard]] const glm::vec3& GetInitialVelocity() const noexcept { return _initialVelocity; }
    [[nodiscard]] const glm::vec3& GetVelocityVariance() const noexcept { return _velocityVariance; }
    [[nodiscard]] const glm::vec3& GetGravity() const noexcept { return _gravity; }
    [[nodiscard]] const glm::vec4& GetStartColor() const noexcept { return _startColor; }
    [[nodiscard]] const glm::vec4& GetEndColor() const noexcept { return _endColor; }
    [[nodiscard]] float GetParticleSize() const noexcept { return _particleSize; }
    [[nodiscard]] bool GetTriggerOnLand() const noexcept { return _triggerOnLand; }
    [[nodiscard]] bool GetTriggerOnDamage() const noexcept { return _triggerOnDamage; }
    [[nodiscard]] bool GetTriggerOnAttack() const noexcept { return _triggerOnAttack; }
    [[nodiscard]] int GetBurstCount() const noexcept { return _burstCount; }

    [[nodiscard]] float& GetEmissionAccumulator() noexcept { return _emissionAccumulator; }
    [[nodiscard]] std::mt19937& GetRng() noexcept { return _rng; }

    [[nodiscard]] float RandomFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(_rng);
    }

    void TriggerBurst(int count) noexcept { _pendingBurst += count; }
    [[nodiscard]] int GetPendingBurst() const noexcept { return _pendingBurst; }
    void ClearPendingBurst() noexcept { _pendingBurst = 0; }
};
