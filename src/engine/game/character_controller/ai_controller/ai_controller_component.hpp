#pragma once
#include <memory>

#include "../character_controller_settings.hpp"
#include "../../../entity/entity.hpp"
#include "../../grid/grid_component.hpp"
#include "../../../renderer/transform/transform_component.hpp"
#include "../../../renderer/spine/spine_renderer_component.hpp"
#include "../../../nav_mesh/astar_finder_component.hpp"


class AiController final : public Component{
private:

    std::weak_ptr<Transform> _transform;
    std::weak_ptr<Transform> _target;
    std::weak_ptr<SpineRenderer> _renderer{};
    std::weak_ptr<AStarFinderComponent> _astarFinder{};
    std::weak_ptr<GridComponent> _gridComponent{};
    CharacterControllerSettings _characterSettings{};
    std::weak_ptr<CharacterMovementComponent> _characterMovementComponent{};

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

    bool TryToHit();


    void SetLookAt(const glm::vec3 &lookAt) const;

public:
    explicit AiController(const std::weak_ptr<Entity> &entity);

    void SetCharacterControllerSettings(const CharacterControllerSettings &settings) noexcept {
        _characterSettings = settings;
    }

    void SetTarget(const std::weak_ptr<Transform> &target) noexcept {
        _target = target;
    }

    void SetCharacterMovementComponent(const std::weak_ptr<CharacterMovementComponent> &characterMovementComponent) noexcept {
        _characterMovementComponent = characterMovementComponent;
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