#include "ai_controller.hpp"

void AiController::SetFaceRight(const bool &isFaceRight) {
    if (const auto render = _renderer.lock()) {
        render->SetFaceRight(isFaceRight);
    }
}

glm::vec2 AiController::GetCenter() const noexcept {
    if (const auto tr = _transform.lock()) {
        const auto ground = tr->GetPosition();
        return  {ground.x, ground.y + _halfCharacterSize.y};
    }

    return glm::vec2{0.0f, 0.0f};
}

bool AiController::IsHitDir(glm::vec2 position, glm::vec2 dir, glm::vec2 &hitPos) const {

    if (const auto world = _world.lock()) {

        const glm::vec3 pointA(position.x, position.y, 0);    // Start of ray
        const glm::vec3 pointB(position.x + dir.x, position.y + dir.y, 0);   // End of ray

        const auto result = world->RayCast( pointA, pointB);

        hitPos = glm::vec2(result.Point.x, result.Point.y);

        return result.IsHit;
    }

    return false;
}

bool AiController::IsHitUp(glm::vec2 &hitPos) const {
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

bool AiController::IsHitLeft(glm::vec2 &hitPos) const {
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

bool AiController::IsHitRight(glm::vec2 &hitPos) const {
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

bool AiController::IsGrounded(glm::vec2 &hitPos) const {
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

AiController::AiController(const std::weak_ptr<Entity> &entity): Component(entity) {
    _transform = _entity.lock()->GetComponent<Transform>();
}

void AiController::SetLookAt(const glm::vec3 &lookAt) const {
    if (const auto& render = _renderer.lock()) {
        render->LookAt(lookAt, "test");
    }
}

void AiController::SetSpineRenderer(const std::weak_ptr<SpineRenderer> &spineRenderer) noexcept {
    _renderer = spineRenderer;
    _renderer.lock()->SetAnimation(0, "run", true, false);
}

void AiController::Update([[maybe_unused]] const float &deltaTime) {

    auto position = _transform.lock()->GetPosition();
    const auto targetPos = _target.lock()->GetPosition();
    glm::vec2 hitPos{};


    if (targetPos.x < position.x) {
        _velocity.x += -_characterSettings.AccelerationSpeed;
        if (_velocity.x < -_characterSettings.MaxMovementSpeed)
            _velocity.x = -_characterSettings.MaxMovementSpeed;
        SetFaceRight(true);

    } else  {
        _velocity.x += _characterSettings.AccelerationSpeed;
        if (_velocity.x > _characterSettings.MaxMovementSpeed)
            _velocity.x = _characterSettings.MaxMovementSpeed;
        SetFaceRight(false);
    }

    if (IsGrounded(hitPos)) {

        const float diff = std::abs(hitPos.y - position.y);
        if (diff > 0.01) {
            position.y = hitPos.y;
        }
        _velocity.y = 0;
    } else {
        _velocity.x *= _characterSettings.AirControl;
        _velocity.y = -(_characterSettings.JumpHeigh / _characterSettings.JumpDuration) * _characterSettings.JumpDownMultiplier;
    }


    if (IsHitLeft(hitPos)) {
        if (_velocity.x < 0)
            _velocity.x = 0;
    }

    if (IsHitRight(hitPos)) {
        if (_velocity.x > 0)
            _velocity.x = 0;
    }

    if (glm::distance(targetPos, position) < 1) {
        _velocity = glm::vec3(0, 0, 0);
        SetAnimation(0, "hit", true);
    }  else {
        SetAnimation(0, "run", true);
    }

    _transform.lock()->SetPosition(position + (_velocity * deltaTime));
}
