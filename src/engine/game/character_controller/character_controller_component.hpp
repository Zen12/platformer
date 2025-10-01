#pragma once
#include "character_controller_settings.hpp"
#include "../../entity/entity.hpp"
#include "../../renderer/transform/transform_component.hpp"
#include "../../system/input_system.hpp"
#include "../../physics/physics_world.hpp"
#include "../../renderer/spine/spine_renderer_component.hpp"
#include "../../renderer/render_pipeline.hpp"
#include <glm/gtx/rotate_vector.hpp>

#include "movement/character_movement_component.hpp"

class CharacterController final : public Component {
private:
    std::weak_ptr<ParticleEmitterComponent> _particles;
    std::weak_ptr<InputSystem> _inputSystem;
    std::weak_ptr<RenderPipeline> _renderPipeline;
    std::weak_ptr<Transform> _transform;
    std::weak_ptr<PhysicsWorld> _world{};
    std::weak_ptr<SpineRenderer> _renderer{};
    std::weak_ptr<CharacterMovementComponent> _characterMovement;
    CharacterControllerSettings _characterSettings{};

    std::map<size_t,std::tuple<std::string, bool>> _animationValue;
    bool _isRight = false;


private:
    void AppendAnimation(const size_t &index, const std::string &animation, const bool &isLoop) const;

    void SetAnimation(const size_t &index, const std::string &animation, const bool &isLoop, const bool &isReverse);

    void SetFaceRight(const bool& isFaceRight) const noexcept;

    void UpdateInternal(InputSystem *input, const Transform *transform);

    void SetLookAt(const glm::vec3 &lookAt) const;

    [[nodiscard]] glm::vec3 GetMousePosition() const;

    void Shoot(const glm::vec3 &position) const;

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

    void SetSpineRenderer(const std::weak_ptr<SpineRenderer> &spineRenderer) noexcept;

    void Update(const float& deltaTime) override;
};
