#include "character_controller_component.hpp"

#include "../health/health_component.hpp"

#define CHARACTER_CONTROLLER_DEBUG 0

void CharacterController::AppendAnimation(const size_t &index, const std::string &animation, const bool &isLoop) const {

    if (const auto render = _renderer.lock()) {
        render->AppendAnimation(index, animation, isLoop);
    }
}
void CharacterController::SetAnimation(const size_t &index, const std::string &animation, const bool &isLoop, const bool &isReverse) {

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

void CharacterController::SetFaceRight(const bool &isFaceRight) const noexcept {
    if (const auto render = _renderer.lock()) {
        render->SetFaceRight(isFaceRight);
    }
}

void CharacterController::UpdateInternal(InputSystem *input, const Transform *transform) {

    if (const auto movement = _characterMovement.lock()) {
        const auto position = transform->GetPosition();
        const auto velocity = movement->GetVelocity();
        glm::vec3 direction = glm::vec3(0);

        if (input->IsKeyPressing(InputKey::A) || input->IsKeyPress(InputKey::A)) {
            direction = glm::vec3(-1, 0, 0);
        } else if (input->IsKeyPressing(InputKey::D) || input->IsKeyPress(InputKey::D)) {
            direction = glm::vec3(1, 0, 0);
        }

        if (input->IsKeyPress(InputKey::Space)) {
            direction.y = 1;
        }

        const auto mouseWorldPosition = GetMousePosition();
        _isRight = mouseWorldPosition.x > position.x;
        SetFaceRight(_isRight);

        if (velocity.y > 0.01)
            SetAnimation(0, _renderer.lock()->GetJumpAnimation(), true, false);
        else if (std::abs(velocity.x) > 0.01) {
            const bool isCorrectDirection =  _isRight == velocity.x > 0.0f;
            SetAnimation(0, _renderer.lock()->GetMoveAnimation(), true, !isCorrectDirection);
        }
        else
            SetAnimation(0, _renderer.lock()->GetIdleAnimation(), true, false);


        if (input->IsMousePress(MouseButton::Left)) {
            Shoot(mouseWorldPosition);
        }

        SetLookAt(mouseWorldPosition);
        movement->SetDirection(direction);
    }
}

CharacterController::CharacterController(const std::weak_ptr<Entity> &entity): Component(entity) {
    _transform = _entity.lock()->GetComponent<Transform>();
}

void CharacterController::SetLookAt(const glm::vec3 &lookAt) const {
    if (const auto render = _renderer.lock()) {
        render->LookAt(lookAt, "crosshair");
    }
}

glm::vec3 CharacterController::GetMousePosition() const {

    if (const auto input = _inputSystem.lock()) {
        if (const auto render = _renderPipeline.lock()) {
            const auto mousePosition = input->GetMouseScreenSpace();
            return render->ScreenToWorldPoint(mousePosition);
        }
    }
    return {0.0f, 0.0f, 0.0f};
}

void CharacterController::Shoot(const glm::vec3 &lookAt) const {
    if (const auto world = _world.lock()) {
        if (const auto render = _renderer.lock()) {
            const auto startPosition = render->GetBonePosition("gun-tip");
            const auto dir = lookAt - startPosition;
            const auto result = world->RayCast(startPosition, startPosition + dir * 1000.0f, 1);

            if (const auto particles = _particles.lock()) {
                constexpr glm::vec3 axis(0.0f, 0.0f, 1.0f); // Z axis
                constexpr float angle1 = glm::radians(30.0f);
                constexpr float angle2 = glm::radians(-30.0f);

                const glm::vec3 rotated1 = glm::rotate(dir, angle1, axis);
                const glm::vec3 rotated2 = glm::rotate(dir, angle2, axis);

                constexpr float speed = 1.0f;

                particles->OverrideDataVelocity(rotated1 * speed, rotated2 * speed);
                particles->Emit(startPosition);
            }

#ifndef NDEBUG
#if CHARACTER_CONTROLLER_DEBUG
            DebugLines::AddLine(startPosition, lookAt);
#endif
#endif

            if (result.IsHit) {
                if (const auto rigid = result.Rigidbody.lock()) {
                    if (const auto entity = rigid->GetEntity().lock()) {
                        if (const auto health = entity->GetComponent<HealthComponent>().lock()) {
                            health->DecreaseHealth(_characterSettings.Damage);
                        }
                    }
                    if (const auto particles = _particles.lock()) {
                        const glm::vec3 dir = result.Normal;
                        constexpr glm::vec3 axis(0.0f, 0.0f, 1.0f); // Z axis
                        constexpr float angle1 = glm::radians(30.0f);
                        constexpr float angle2 = glm::radians(-30.0f);

                        const glm::vec3 rotated1 = glm::rotate(dir, angle1, axis);
                        const glm::vec3 rotated2 = glm::rotate(dir, angle2, axis);

                        constexpr float speed = 1.0f;

                        particles->OverrideDataVelocity(rotated1 * speed, rotated2 * speed);
                        particles->Emit(result.Point);
                    }
                }
            }
        }
    }
}

void CharacterController::SetSpineRenderer(const std::weak_ptr<SpineRenderer> &spineRenderer) noexcept {
    _renderer = spineRenderer;

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

void CharacterController::Update([[maybe_unused]] const float &deltaTime) {

    if (const auto input = _inputSystem.lock())
        if (const auto transform = _transform.lock())
            if (const auto world = _world.lock())
                if (const auto b2World = world->GetWorld().lock())
                    UpdateInternal(input.get(), transform.get());

}
