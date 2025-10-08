#pragma once
#include "character_controller_settings.hpp"
#include "../../entity/entity.hpp"
#include "../../renderer/transform/transform_component.hpp"
#include "../../system/input_system.hpp"
#include "../../physics/physics_world.hpp"
#include "../../renderer/spine/spine_renderer_component.hpp"
#include "../../renderer/render_pipeline.hpp"
#include <glm/gtx/rotate_vector.hpp>

#include "character_animation/character_animation_component.hpp"
#include "movement/character_movement_component.hpp"
#include "shoot/shoot_component.hpp"

class CharacterController final : public Component {
private:
    std::weak_ptr<ParticleEmitterComponent> _particles;
    std::weak_ptr<InputSystem> _inputSystem;
    std::weak_ptr<RenderPipeline> _renderPipeline;
    std::weak_ptr<Transform> _transform;
    std::weak_ptr<PhysicsWorld> _world{};
    std::weak_ptr<CharacterMovementComponent> _characterMovement;
    std::weak_ptr<CharacterAnimationComponent> _animation;
    std::weak_ptr<ShootComponent> _shootComponent;
    CharacterControllerSettings _characterSettings{};

private:

    void UpdateInternal(InputSystem *input) const;

    [[nodiscard]] glm::vec3 GetMousePosition() const;

public:
    explicit CharacterController(const std::weak_ptr<Entity> &entity);

    void SetInputSystem(const std::weak_ptr<InputSystem> &inputSystem) noexcept {
        _inputSystem = inputSystem;
    }

    void SetCharacterControllerSettings(const CharacterControllerSettings &settings) noexcept {
        _characterSettings = settings;
    }

    void SetPhysicsWorld(const std::weak_ptr<PhysicsWorld> &physicsWorld) noexcept {
        _world = physicsWorld;
    }

    void SetParticles(const std::weak_ptr<ParticleEmitterComponent> &particles) noexcept {
        _particles = particles;
    }

    void SetRenderPipeline(const std::weak_ptr<RenderPipeline> &renderPipeline) noexcept {
        _renderPipeline = renderPipeline;
    }

    void SetCharacterMovement(const std::weak_ptr<CharacterMovementComponent> &characterMovement) noexcept {
        _characterMovement = characterMovement;
    }

    void SetAnimation(const std::weak_ptr<CharacterAnimationComponent> &animation) noexcept {
        _animation = animation;
    }

    void SetShootComponent(const std::weak_ptr<ShootComponent> &shootComponent) noexcept;

    void SetTransform(const std::weak_ptr<Transform> &transform) noexcept {
        _transform = transform;
    }

    void Update(const float& deltaTime) override;
};
