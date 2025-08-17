#include "character_controller.hpp"

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

void CharacterController::ResetJump() noexcept {
    _velocity.y = 0;
    _currentJumpDuration = _characterSettings.JumpDuration + 1;
}

glm::vec2 CharacterController::GetCenter() const noexcept {
    if (const auto tr = _transform.lock()) {
        const auto ground = tr->GetPosition();
        return  {ground.x, ground.y + _halfCharacterSize.y};
    }

    return glm::vec2{0.0f, 0.0f};
}

bool CharacterController::IsHitDir(glm::vec2 position, glm::vec2 dir, glm::vec2 &hitPos) const {

        if (const auto world = _world.lock()) {

            const glm::vec3 pointA(position.x, position.y, 0);    // Start of ray
            const glm::vec3 pointB(position.x + dir.x, position.y + dir.y, 0);   // End of ray

            const auto result = world->RayCast( pointA, pointB);

            hitPos = glm::vec2(result.Point.x, result.Point.y);

            return result.IsHit;
        }

        return false;
    }

bool CharacterController::IsHitUp(glm::vec2 &hitPos) const {
    const auto center = GetCenter();

    if (IsHitDir(
                 center + glm::vec2{_halfCharacterSize.x, _fourthPartCharacterSize.y},
                 glm::vec2{0.0f, _fourthPartCharacterSize.y}, hitPos))
        return true;

    if (IsHitDir(
                 center + glm::vec2{-_halfCharacterSize.x, _fourthPartCharacterSize.y},
                 glm::vec2{0.0f, _fourthPartCharacterSize.y}, hitPos))
        return true;


    return false;
}

bool CharacterController::IsHitLeft(glm::vec2 &hitPos) const {
    const auto center = GetCenter();

    if (IsHitDir(
                 center + glm::vec2(-_fourthPartCharacterSize.x, -_fourthPartCharacterSize.y),
                 glm::vec2(-_fourthPartCharacterSize.y, 0.0f), hitPos))
        return true;

    if (IsHitDir(
                 center + glm::vec2(-_fourthPartCharacterSize.x, _fourthPartCharacterSize.y),
                 glm::vec2(-_fourthPartCharacterSize.y, 0.0f), hitPos))
        return true;


    return false;
}

bool CharacterController::IsHitRight(glm::vec2 &hitPos) const {
    const auto center = GetCenter();

    if (IsHitDir(
                 center + glm::vec2(_fourthPartCharacterSize.x, -_fourthPartCharacterSize.y),
                 glm::vec2(_fourthPartCharacterSize.y, 0.0f), hitPos))
        return true;

    if (IsHitDir(
                 center + glm::vec2(_fourthPartCharacterSize.x, _fourthPartCharacterSize.y),
                 glm::vec2(_fourthPartCharacterSize.y, 0.0f), hitPos))
        return true;

    return false;
}

bool CharacterController::IsGrounded(glm::vec2 &hitPos) const {
    const auto center = GetCenter();

    if (IsHitDir(
                 center + glm::vec2{_halfCharacterSize.x, -_fourthPartCharacterSize.y},
                 glm::vec2{0.0f, -_fourthPartCharacterSize.y}, hitPos))
        return true;

    if (IsHitDir(
                 center + glm::vec2{-_halfCharacterSize.x, -_fourthPartCharacterSize.y},
                 glm::vec2{0.0f, -_fourthPartCharacterSize.y}, hitPos))
        return true;


    return false;
}

void CharacterController::UpdateInternal(const float &deltaTime, InputSystem *input, Transform *transform) {

    auto position = transform->GetPosition();

    glm::vec2 hitPos{};

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

    if (IsGrounded(hitPos)) {
        ResetJump();

        const float diff = std::abs(hitPos.y - position.y);
        if (diff > 0.01) {
            position.y = hitPos.y;
        }

        if (input->IsKeyPress(InputKey::Space)) {
            _currentJumpDuration = 0;
            _velocity.y = _characterSettings.JumpHeigh / _characterSettings.JumpDuration;
        }

    } else {
        _velocity.x *= _characterSettings.AirControl;
        _currentJumpDuration += deltaTime;

        if (_currentJumpDuration > _characterSettings.JumpDuration) {
            _velocity.y = -(_characterSettings.JumpHeigh / _characterSettings.JumpDuration) * _characterSettings.JumpDownMultiplier;
        }
    }


    if (IsHitLeft(hitPos)) {
        if (_velocity.x < 0)
            _velocity.x = 0;
    }

    if (IsHitRight(hitPos)) {
        if (_velocity.x > 0)
            _velocity.x = 0;
    }

    if (IsHitUp(hitPos)) {
        if (_velocity.y > 0) {
            ResetJump();
        }
    }

    const auto mouseWorldPosition = GetMousePosition();
    _isRight = mouseWorldPosition.x > position.x;
    SetFaceRight(_isRight);

    if (_velocity.y > 0.01)
        SetAnimation(0, "walk", true, false);
    else if (std::abs(_velocity.x) > 0.01) {
        const bool isCorrectDirection =  _isRight == _velocity.x > 0.0f;
        SetAnimation(0, "run", true, !isCorrectDirection);
    }
    else
        SetAnimation(0, "idle", true, false);


   // if (input->IsMousePress(MouseButton::Left)) {
        Shoot(mouseWorldPosition);
    //}

    transform->SetPosition(position + (_velocity * deltaTime));
    SetLookAt(mouseWorldPosition);
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

void CharacterController::Shoot(const glm::vec3 &lookAt) {
    if (const auto world = _world.lock()) {
        const auto startPosition = _renderer.lock()->GetBonePosition("gun-tip");
        const auto result = world->RayCast(startPosition, lookAt);
        auto finalPosition = lookAt;

        if (result.IsHit)
            finalPosition = result.Point;

#ifndef NDEBUG
#if CHARACTER_CONTROLLER_DEBUG
        DebugLines::AddLine(startPosition, finalPosition);
#endif
#endif

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

void CharacterController::Update(const float &deltaTime) {

    if (const auto input = _inputSystem.lock())
        if (const auto transform = _transform.lock())
            if (const auto world = _world.lock())
                if (const auto b2World = world->GetWorld().lock())
                    UpdateInternal(deltaTime, input.get(), transform.get());

}
