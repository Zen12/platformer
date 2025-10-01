#pragma once

#include "../../../entity/entity.hpp"
#include "../../../renderer/spine/spine_renderer_component.hpp"
#include "../movement/character_movement_component.hpp"


class CharacterAnimationComponent final : public Component {
private:
    std::weak_ptr<CharacterMovementComponent> _characterMovementComponent;
    std::weak_ptr<SpineRenderer> _renderer;
    std::map<size_t,std::tuple<std::string, bool>> _animationValue;
    std::weak_ptr<Transform> _transform;
    glm::vec3 _lookAt{0};

public:
    explicit CharacterAnimationComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void SetCharacterMovementComponent(const std::weak_ptr<CharacterMovementComponent> &characterMovementComponent) {
        _characterMovementComponent = characterMovementComponent;
    }

    void SetRenderer(const std::weak_ptr<SpineRenderer> &renderer) {
        _renderer = renderer;

        if (const auto render = _renderer.lock()) {
            render->SetTransition("walk", "idle", 0.3);
            render->SetTransition("walk", "run", 0.3);

            render->SetTransition("idle", "walk", 0.3);
            render->SetTransition("idle", "run", 0.3);

            render->SetTransition("run", "idle", 0.3);
            render->SetTransition("run", "walk", 0.3);
        }

        SetAnimation(1, "aim", true, false);
    }


    void Update([[maybe_unused]] const float &deltaTime) override {
        if (const auto &transform = _transform.lock()) {
            if (const auto movement = _characterMovementComponent.lock()) {
                const auto velocity = movement->GetVelocity();

                const auto _isRight = _lookAt.x > transform->GetPosition().x;
                SetFaceRight(_isRight);

                if (velocity.y > 0.01)
                    SetAnimation(0, _renderer.lock()->GetJumpAnimation(), true, false);
                else if (std::abs(velocity.x) > 0.01) {
                    const bool isCorrectDirection =  _isRight == velocity.x > 0.0f;
                    SetAnimation(0, _renderer.lock()->GetMoveAnimation(), true, !isCorrectDirection);
                }
                else
                    SetAnimation(0, _renderer.lock()->GetIdleAnimation(), true, false);

                if (const auto render = _renderer.lock()) {
                    render->LookAt(_lookAt, "crosshair");
                }
            }
        }
    }

    void SetAnimation(const size_t &index, const std::string &animation, const bool &isLoop, const bool &isReverse) {

        if (_animationValue.find(index) != _animationValue.end()) {
            const auto [animationCache, isReverseCache ] = _animationValue[index];
            if (animationCache == animation && isReverseCache == isReverse  ) {
                return;
            }
        }

        _animationValue[index] = {animation, isReverse};
        if (const auto render = _renderer.lock()) {
            render->SetAnimation(index, animation, isLoop, isReverse);
        }
    }

    void SetFaceRight(const bool &isFaceRight) const noexcept {
        if (const auto render = _renderer.lock()) {
            render->SetFaceRight(isFaceRight);
        }
    }

    [[nodiscard]] glm::vec3 GetBonePosition(const std::string &boneName) const noexcept {

        if (const auto render = _renderer.lock()) {
            return render->GetBonePosition(boneName);
        }
        return glm::vec3{0};
    }

    void SetTransform(const std::weak_ptr<Transform> &transform) noexcept {
        _transform = transform;
    }

    void SetLookAt(const glm::vec3 &lookAt) noexcept {
        _lookAt = lookAt;
    }
};
