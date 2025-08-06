#pragma once
#include "character_controller_settings.h"
#include "../components/entity.hpp"
#include "../components/transforms/transform.hpp"
#include "../system/input_system.hpp"

/*
Movement
Acceleration
MaxSpeed
Deceleration

Camera:
DampingX
DampingY
LookAhead

Jump:
JumpHeigh
Duration
JumpDownMultiplyer

Assists:
Coyote Time - time in air to jump
Jump Buffer - time to remember jump-button-down so when the ground it will jump

Juice:
Particles:
Run/Jump/Land

Squash&Stretch
Jump/Land
 */

class CharacterController final : public Component{
private:
    std::weak_ptr<InputSystem> _inputSystem;
    std::weak_ptr<Transform> _transform;
    std::weak_ptr<PhysicsWorld> _world;
    CharacterControllerSettings _characterSettings{};


    float _currentJumpDuration{100000};

    glm::vec3 _velocity{0};

private:

    [[nodiscard]] bool IsGrounded(const b2World *world, const Transform *transform, glm::vec2 &hitPos) const {
        const auto position = transform->GetPosition();
        b2Vec2 pointA(position.x, position.y + 0.2f);    // Start of ray
        b2Vec2 pointB(position.x, position.y);   // End of ray

        RayCastClosestCallback callback;
        world->RayCast(&callback, pointA, pointB);

        hitPos = glm::vec2(callback.Point.x, callback.Point.y);

        return callback.Hit;
    }

    void UpdateInternal(const float &deltaTime, InputSystem *input, Transform *transform, b2World* world) {

        auto position = transform->GetPosition();

        glm::vec2 hitPos{};

        if (IsGrounded(world, transform, hitPos) && _velocity.y < 0.001f) {
            _velocity.y = 0;
            position.y = hitPos.y;
            _currentJumpDuration = _characterSettings.JumpDuration + 1;
            if (input->IsKeyPressing(InputKey::A) || input->IsKeyPress(InputKey::A)) {
                _velocity.x = -_characterSettings.MovementSpeed;
            } else if (input->IsKeyPressing(InputKey::D) || input->IsKeyPress(InputKey::D)) {
                _velocity.x = _characterSettings.MovementSpeed;
            } else {
                _velocity.x = 0;
            }

            if (input->IsKeyPress(InputKey::Space)) {
                _currentJumpDuration = 0;
                _velocity.y = _characterSettings.JumpHeigh / _characterSettings.JumpDuration;
            }

        } else {
            _currentJumpDuration += deltaTime;

            if (_currentJumpDuration > _characterSettings.JumpDuration) {
                _velocity.y = -(_characterSettings.JumpHeigh / _characterSettings.JumpDuration) * _characterSettings.JumpDownMultiplier;
            }
        }

        transform->SetPosition(position + (_velocity * deltaTime));
    }

public:
    explicit CharacterController(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
        _transform = _entity.lock()->GetComponent<Transform>();
    }

    void SetInputSystem(const std::weak_ptr<InputSystem> &inputSystem) noexcept {
        _inputSystem = inputSystem;
    }

    void SetCharacterControllerSettings(const CharacterControllerSettings &settings) noexcept {
        _characterSettings = settings;
    }

    void SetPhysicsWorld(const std::weak_ptr<PhysicsWorld> &physicsWorld) noexcept {
        _world = physicsWorld;
    }


    void Update(const float& deltaTime) override {

        if (const auto input = _inputSystem.lock())
            if (const auto transform = _transform.lock())
                if (const auto world = _world.lock())
                    if (const auto b2World = world->GetWorld().lock())
                        UpdateInternal(deltaTime, input.get(), transform.get(), b2World.get());

    }

};
