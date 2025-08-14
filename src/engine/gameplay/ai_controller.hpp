#pragma once
#include <memory>

#include "character_controller_settings.h"
#include "../components/entity.hpp"
#include "../components/transforms/transform.hpp"
#include "../system/input_system.hpp"
#include "box2d/b2_world.h"
#include "../physics/physics_world.hpp"
#include "../physics/raycast.hpp"
#include "../components/spine_renderer.hpp"



class AiController final : public Component{
private:

    std::weak_ptr<InputSystem> _inputSystem;
    std::weak_ptr<Transform> _transform;
    std::weak_ptr<PhysicsWorld> _world{};
    std::weak_ptr<SpineRenderer> _renderer{};
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

    std::map<size_t, std::string> _animationValue;
    bool _isRight = false;


private:

    void SetAnimation(const size_t &index, const std::string &animation, const bool &isLoop) {

        if (_animationValue.find(index) != _animationValue.end()) {
            if (_animationValue[index] == animation) {
                return;
            }
        }

        _animationValue[index] = animation;
        if (const auto render = _renderer.lock()) {
            render->SetAnimation(index, animation, isLoop);
        }
    }

    void SetFaceRight(const bool& isFaceRight);

    void ResetJump() noexcept;

    [[nodiscard]] glm::vec2 GetCenter() const noexcept;

    [[nodiscard]] bool IsHitDir(const b2World *world, glm::vec2 position, glm::vec2 dir, glm::vec2 &hitPos) const;

    [[nodiscard]] bool IsHitUp(const b2World *world, glm::vec2 &hitPos) const;

    [[nodiscard]] bool IsHitLeft(const b2World *world, glm::vec2 &hitPos) const;

    [[nodiscard]] bool IsHitRight(const b2World *world, glm::vec2 &hitPos) const;

    [[nodiscard]] bool IsGrounded(const b2World *world, glm::vec2 &hitPos) const;

    void UpdateInternal(const float &deltaTime, InputSystem *input, Transform *transform, b2World* world);

    void SetLookAt(glm::vec2 &lookAt);

public:
    explicit AiController(const std::weak_ptr<Entity> &entity);

    void SetInputSystem(const std::weak_ptr<InputSystem> &inputSystem) noexcept {
        _inputSystem = inputSystem;
    }

    void SetCharacterControllerSettings(const CharacterControllerSettings &settings) noexcept {
        _characterSettings = settings;
    }

    void SetPhysicsWorld(const std::weak_ptr<PhysicsWorld> &physicsWorld) noexcept {
        _world = physicsWorld;
    }

    void SetSpineRenderer(const std::weak_ptr<SpineRenderer> &spineRenderer) noexcept;

    void Update(const float& deltaTime) override;
};
