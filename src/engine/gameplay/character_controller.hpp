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
    CharacterControllerSettings _characterSettings{1.0, 1.0};

private:

    [[nodiscard]] bool IsGrounded(const b2World *world, const Transform *transform) const {
        const auto position = transform->GetPosition();
        b2Vec2 pointA(position.x, position.y + 0.1f);    // Start of ray
        b2Vec2 pointB(position.x, position.y);   // End of ray

        RayCastClosestCallback callback;
        world->RayCast(&callback, pointA, pointB);

        return callback.Hit;
    }

    void UpdateInternal(const float &deltaTime, InputSystem *input, Transform *transform, b2World* world) const {
        auto position = transform->GetPosition();
        const auto speed = _characterSettings.Speed;

        if (IsGrounded(world, transform)) {
            if (input->IsKeyPressing(InputKey::A) || input->IsKeyPress(InputKey::A)) {
                position.x -= speed * deltaTime;
            }
            if (input->IsKeyPressing(InputKey::D) || input->IsKeyPress(InputKey::D)) {
                position.x += speed * deltaTime;
            }

        } else {
            position.y -= speed * deltaTime; // fall
        }

        transform->SetPosition(position);
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


    void Update(const float& deltaTime) const override {

        if (const auto input = _inputSystem.lock())
            if (const auto transform = _transform.lock())
                if (const auto world = _world.lock())
                    if (const auto b2World = world->GetWorld().lock())
                        UpdateInternal(deltaTime, input.get(), transform.get(), b2World.get());

    }

};
