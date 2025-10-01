#include "character_movement_component.hpp"

void CharacterMovementComponent::ResetJump() noexcept {
    _velocity.y = 0;
    _currentJumpDuration = _settings.JumpDuration + 1;
}

glm::vec2 CharacterMovementComponent::GetCenter() const noexcept {
    if (const auto tr = _transform.lock()) {
        const auto ground = tr->GetPosition();
        return  {ground.x, ground.y + _halfCharacterSize.y};
    }

    return glm::vec2{0.0f, 0.0f};
}

bool CharacterMovementComponent::IsHitUp(glm::vec2 &hitPos) const {
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

bool CharacterMovementComponent::IsHitDir(glm::vec2 position, glm::vec2 dir, glm::vec2 &hitPos) const {

    if (const auto world = _world.lock()) {

        const glm::vec3 pointA(position.x, position.y, 0);    // Start of ray
        const glm::vec3 pointB(position.x + dir.x, position.y + dir.y, 0);   // End of ray

        const auto result = world->RayCast( pointA, pointB, 1);

        hitPos = glm::vec2(result.Point.x, result.Point.y);

        return result.IsHit;
    }

    return false;
}

bool CharacterMovementComponent::IsHitLeft(glm::vec2 &hitPos) const {
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

bool CharacterMovementComponent::IsHitRight(glm::vec2 &hitPos) const {
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

bool CharacterMovementComponent::IsGrounded(glm::vec2 &hitPos) const {
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

void CharacterMovementComponent::Update(const float &deltaTime) {
    if (const auto& transform = _transform.lock()) {

        auto position = transform->GetPosition();

        glm::vec2 hitPos{};

        if (_currentDirection.x < -0.1f) {
            _velocity.x += -_settings.AccelerationSpeed;
            if (_velocity.x < -_settings.MaxMovementSpeed)
                _velocity.x = -_settings.MaxMovementSpeed;
        } else if (_currentDirection.x > 0.1f){
            _velocity.x += _settings.AccelerationSpeed;
            if (_velocity.x > _settings.MaxMovementSpeed)
                _velocity.x = _settings.MaxMovementSpeed;
        } else {
            const float sign =  std::signbit(_velocity.x) ? -1.0f : 1.0f;
            float absValue = std::abs(_velocity.x);
            absValue -= _settings.Deceleration;
            if (absValue < 0) {
                absValue = 0;
            }
            _velocity.x = sign * absValue;
        }

        if (IsGrounded(hitPos)) {
            ResetJump();

            if (const float diff = std::abs(hitPos.y - position.y); diff > 0.01) {
                position.y = hitPos.y;
            }

            if (_currentDirection.y > 0.1f) {
                _currentJumpDuration = 0;
                _velocity.y = _settings.JumpHeigh / _settings.JumpDuration;
            }

        } else {
            _velocity.x *= _settings.AirControl;
            _currentJumpDuration += deltaTime;

            if (_currentJumpDuration > _settings.JumpDuration) {
                _velocity.y = -(_settings.JumpHeigh / _settings.JumpDuration) * _settings.JumpDownMultiplier;
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

        transform->SetPosition(position + (_velocity * deltaTime));
    }
}
