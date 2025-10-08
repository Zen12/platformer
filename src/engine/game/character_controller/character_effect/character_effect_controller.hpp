#pragma once
#include "../../../entity/entity.hpp"
#include "../../../renderer/particles/particle_emitter_component.hpp"
#include "../shoot/shoot_component.hpp"
#include <glm/gtx/rotate_vector.hpp>

class CharacterEffectController final : public Component {
private:
    std::weak_ptr<ParticleEmitterComponent> _particles;

public:
    explicit CharacterEffectController(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void SetParticles(const std::weak_ptr<ParticleEmitterComponent> &particles) noexcept {
        _particles = particles;
    }

    void EmitMuzzleFlash(const ShootResult &shootResult) const;
    void EmitImpact(const ShootResult &shootResult) const;

    void Update(const float &deltaTime) override;
};
