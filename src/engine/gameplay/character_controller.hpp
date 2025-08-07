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

    glm::vec2 _characterSize{1.0f, 2.0f};
    glm::vec2 _halfCharacterSize{0.5f, 1.0f};
    glm::vec2 _fourthPartCharacterSize{0.25f, 0.5f};

    glm::vec2 _topRightCorner = {_characterSize.x, _characterSize.y};
    glm::vec2 _topLeftCorner = {-_characterSize.x, _characterSize.y};
    glm::vec2 _downRightCorner = {_characterSize.x, -_characterSize.y};
    glm::vec2 _downLeftCorner = {_characterSize.x, -_characterSize.y};


    float _currentJumpDuration{100000};

    glm::vec3 _velocity{0};

private:

    void ResetJump() noexcept {
        _velocity.y = 0;
        _currentJumpDuration = _characterSettings.JumpDuration + 1;
    }
    [[nodiscard]] glm::vec2 GetCenter() const noexcept {
        if (const auto tr = _transform.lock()) {
            const auto ground = tr->GetPosition();
            return  {ground.x, ground.y + _halfCharacterSize.y};
        }

        return glm::vec2{0.0f, 0.0f};
    }

    [[nodiscard]] bool IsHitDir(const b2World *world, glm::vec2 position, glm::vec2 dir, glm::vec2 &hitPos) const {
        b2Vec2 pointA(position.x, position.y);    // Start of ray
        b2Vec2 pointB(position.x + dir.x, position.y + dir.y);   // End of ray

        RayCastClosestCallback callback;
        world->RayCast(&callback, pointA, pointB);

        hitPos = glm::vec2(callback.Point.x, callback.Point.y);

        return callback.Hit;
    }

    [[nodiscard]] bool IsHitUp(const b2World *world, glm::vec2 &hitPos) const {
        const auto center = GetCenter();

        if (IsHitDir(world,
            center + glm::vec2{_halfCharacterSize.x, _fourthPartCharacterSize.y},
            glm::vec2{0.0f, _fourthPartCharacterSize.y}, hitPos))
            return true;

        if (IsHitDir(world,
              center + glm::vec2{-_halfCharacterSize.x, _fourthPartCharacterSize.y},
              glm::vec2{0.0f, _fourthPartCharacterSize.y}, hitPos))
            return true;


        return false;
    }

    [[nodiscard]] bool IsHitLeft(const b2World *world, glm::vec2 &hitPos) const {
        const auto center = GetCenter();

        if (IsHitDir(world,
            center + glm::vec2(-_fourthPartCharacterSize.x, -_fourthPartCharacterSize.y),
            glm::vec2(-_fourthPartCharacterSize.y, 0.0f), hitPos))
            return true;

        if (IsHitDir(world,
        center + glm::vec2(-_fourthPartCharacterSize.x, _fourthPartCharacterSize.y),
            glm::vec2(-_fourthPartCharacterSize.y, 0.0f), hitPos))
            return true;


        return false;
    }

    [[nodiscard]] bool IsHitRight(const b2World *world, glm::vec2 &hitPos) const {
        const auto center = GetCenter();

        if (IsHitDir(world,
            center + glm::vec2(_fourthPartCharacterSize.x, -_fourthPartCharacterSize.y),
            glm::vec2(_fourthPartCharacterSize.y, 0.0f), hitPos))
            return true;

        if (IsHitDir(world,
        center + glm::vec2(_fourthPartCharacterSize.x, _fourthPartCharacterSize.y),
            glm::vec2(_fourthPartCharacterSize.y, 0.0f), hitPos))
            return true;

        return false;
    }


    [[nodiscard]] bool IsGrounded(const b2World *world, glm::vec2 &hitPos) const {
        const auto center = GetCenter();

        if (IsHitDir(world,
            center + glm::vec2{_halfCharacterSize.x, -_fourthPartCharacterSize.y},
            glm::vec2{0.0f, -_fourthPartCharacterSize.y}, hitPos))
            return true;

        if (IsHitDir(world,
              center + glm::vec2{-_halfCharacterSize.x, -_fourthPartCharacterSize.y},
              glm::vec2{0.0f, -_fourthPartCharacterSize.y}, hitPos))
            return true;


        return false;
    }

    void UpdateInternal(const float &deltaTime, InputSystem *input, Transform *transform, b2World* world) {

        auto position = transform->GetPosition();

        glm::vec2 hitPos{};

        if (IsGrounded(world, hitPos)) {
            ResetJump();

            float diff = std::abs(hitPos.y - position.y);
            if (diff > 0.01) {
                position.y = hitPos.y;
            }
            if (input->IsKeyPressing(InputKey::A) || input->IsKeyPress(InputKey::A)) {
                _velocity.x += -_characterSettings.AccelerationSpeed;
                if (_velocity.x < -_characterSettings.MaxMovementSpeed)
                    _velocity.x = -_characterSettings.MaxMovementSpeed;
            } else if (input->IsKeyPressing(InputKey::D) || input->IsKeyPress(InputKey::D)) {
                _velocity.x += _characterSettings.AccelerationSpeed;
                if (_velocity.x > _characterSettings.MaxMovementSpeed)
                    _velocity.x = _characterSettings.MaxMovementSpeed;
            } else {
                const float sign =  std::signbit(_velocity.x) ? -1.0f : 1.0f;
                float absValue = std::abs(_velocity.x);
                absValue -= _characterSettings.Deceleration;
                if (absValue < 0) {
                    absValue = 0;
                }
                _velocity.x = sign * absValue;
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


        if (IsHitLeft(world, hitPos)) {
            if (_velocity.x < 0)
                _velocity.x = 0;
        }

        if (IsHitRight(world, hitPos)) {
            if (_velocity.x > 0)
                _velocity.x = 0;
        }

        if (IsHitUp(world, hitPos)) {
            if (_velocity.y > 0) {
                ResetJump();
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
