#include "character_animation_controller.hpp"

void CharacterAnimationController::Update([[maybe_unused]] const float &deltaTime) {
    if (const auto animation = _animationComponent.lock()) {
        if (const auto movement = _movementComponent.lock()) {
            if (const auto transform = _transform.lock()) {
                const auto velocity = movement->GetVelocity();
                const auto position = transform->GetPosition();

                const bool isRight = _lookAt.x > position.x;
                animation->SetFaceRight(isRight);

                if (velocity.y > 0.01f) {
                    animation->SetAnimation(0, animation->GetRenderer().lock()->GetJumpAnimation(), true, false);
                } else if (std::abs(velocity.x) > 0.01f) {
                    const bool isCorrectDirection = isRight == (velocity.x > 0.0f);
                    animation->SetAnimation(0, animation->GetRenderer().lock()->GetMoveAnimation(), true, !isCorrectDirection);
                } else {
                    animation->SetAnimation(0, animation->GetRenderer().lock()->GetIdleAnimation(), true, false);
                }

                animation->SetLookAt(_lookAt);
            }
        }
    }
}
