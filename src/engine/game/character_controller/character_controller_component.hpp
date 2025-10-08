#pragma once
#include "character_controller_settings.hpp"
#include "../../entity/entity.hpp"
#include "../../renderer/transform/transform_component.hpp"
#include "../../physics/physics_world.hpp"
#include "../../renderer/spine/spine_renderer_component.hpp"
#include <glm/gtx/rotate_vector.hpp>

#include "character_animation/character_animation_component.hpp"
#include "movement/character_movement_component.hpp"
#include "shoot/shoot_component.hpp"
#include "character_input/character_input_component.hpp"

class CharacterController final : public Component {
private:
    std::weak_ptr<ParticleEmitterComponent> _particles;
    std::weak_ptr<Transform> _transform;
    std::weak_ptr<PhysicsWorld> _world{};
    std::weak_ptr<CharacterMovementComponent> _characterMovement;
    std::weak_ptr<CharacterAnimationComponent> _animation;
    std::weak_ptr<ShootComponent> _shootComponent;
    std::weak_ptr<CharacterInputComponent> _inputComponent;
    CharacterControllerSettings _characterSettings{};

public:
    explicit CharacterController(const std::weak_ptr<Entity> &entity);

    void SetCharacterControllerSettings(const CharacterControllerSettings &settings) noexcept {
        _characterSettings = settings;
    }

    void SetPhysicsWorld(const std::weak_ptr<PhysicsWorld> &physicsWorld) noexcept {
        _world = physicsWorld;
    }

    void SetParticles(const std::weak_ptr<ParticleEmitterComponent> &particles) noexcept {
        _particles = particles;
    }

    void SetCharacterMovement(const std::weak_ptr<CharacterMovementComponent> &characterMovement) noexcept {
        _characterMovement = characterMovement;
    }

    void SetAnimation(const std::weak_ptr<CharacterAnimationComponent> &animation) noexcept {
        _animation = animation;
    }

    void SetShootComponent(const std::weak_ptr<ShootComponent> &shootComponent) noexcept {
        _shootComponent = shootComponent;
    }

    void SetInputComponent(const std::weak_ptr<CharacterInputComponent> &inputComponent) noexcept {
        _inputComponent = inputComponent;
    }

    void SetTransform(const std::weak_ptr<Transform> &transform) noexcept {
        _transform = transform;
    }

    void Update(const float& deltaTime) override;
};
