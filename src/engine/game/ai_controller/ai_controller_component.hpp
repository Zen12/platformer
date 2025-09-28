#pragma once
#include <memory>

#include "../character_controller/character_controller_settings.hpp"
#include "../../components/entity.hpp"
#include "../grid/grid_component.hpp"
#include "../../renderer/transform/transform_component.hpp"
#include "../../system/input_system.hpp"
#include "../../physics/physics_world.hpp"
#include "../../renderer/spine/spine_renderer_component.hpp"
#include "../../nav_mesh/astar_finder_component.hpp"


class AiController final : public Component{
private:

    std::weak_ptr<InputSystem> _inputSystem;
    std::weak_ptr<Transform> _transform;
    std::weak_ptr<Transform> _target;
    std::weak_ptr<PhysicsWorld> _world{};
    std::weak_ptr<SpineRenderer> _renderer{};
    std::weak_ptr<AStarFinderComponent> _astarFinder{};
    std::weak_ptr<GridComponent> _gridComponent{};
    CharacterControllerSettings _characterSettings{};

    glm::vec2 _characterSize{1.0f, 2.0f};
    glm::vec2 _halfCharacterSize{0.5f, 1.0f};
    glm::vec2 _fourthPartCharacterSize{0.25f, 0.5f};

    glm::vec2 _topRightCorner = {_characterSize.x, _characterSize.y};
    glm::vec2 _topLeftCorner = {-_characterSize.x, _characterSize.y};
    glm::vec2 _downRightCorner = {_characterSize.x, -_characterSize.y};
    glm::vec2 _downLeftCorner = {_characterSize.x, -_characterSize.y};



    glm::vec3 _velocity{0};
    float _hitCooldown{0};

    std::map<size_t, std::string> _animationValue;


private:

    void SetAnimation(const size_t &index, const std::string &animation, const bool &isLoop);

    void AppendAnimation(const size_t &index, const std::string &animation, const bool &isLoop) const {
        if (const auto render = _renderer.lock()) {
            render->AppendAnimation(index, animation, isLoop);
        }
    }

    void SetFaceRight(const bool& isFaceRight) const;

    [[nodiscard]] glm::vec2 GetCenter() const noexcept;

    [[nodiscard]] bool IsHitDir(glm::vec2 position, glm::vec2 dir, glm::vec2 &hitPos) const;

    [[nodiscard]] bool IsHitUp(glm::vec2 &hitPos) const;

    [[nodiscard]] bool IsHitLeft(glm::vec2 &hitPos) const;

    [[nodiscard]] bool IsHitRight(glm::vec2 &hitPos) const;

    [[nodiscard]] bool IsGrounded( glm::vec2 &hitPos) const;

    bool TryToHit();


    void SetLookAt(const glm::vec3 &lookAt) const;

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

    void SetTarget(const std::weak_ptr<Transform> &target) noexcept {
        _target = target;
    }

    void SetPathFinder(const std::weak_ptr<AStarFinderComponent> &pathFinder) noexcept {
        _astarFinder = pathFinder;
    }

    void SetGrid(const std::weak_ptr<GridComponent> &grid) noexcept {
        _gridComponent = grid;
    }

    void SetSpineRenderer(const std::weak_ptr<SpineRenderer> &spineRenderer) noexcept;

    void Update(const float& deltaTime) override;
};