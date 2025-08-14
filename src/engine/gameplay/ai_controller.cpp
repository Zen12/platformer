#include "ai_controller.hpp"

void AiController::SetFaceRight(const bool &isFaceRight) {
    if (const auto render = _renderer.lock()) {
        render->SetFaceRight(isFaceRight);
    }
}

void AiController::ResetJump() noexcept {
    _velocity.y = 0;
    _currentJumpDuration = _characterSettings.JumpDuration + 1;
}

glm::vec2 AiController::GetCenter() const noexcept {
    if (const auto tr = _transform.lock()) {
        const auto ground = tr->GetPosition();
        return  {ground.x, ground.y + _halfCharacterSize.y};
    }

    return glm::vec2{0.0f, 0.0f};
}

bool AiController::IsHitDir(const b2World *world, glm::vec2 position, glm::vec2 dir, glm::vec2 &hitPos) const {
    b2Vec2 pointA(position.x, position.y);    // Start of ray
    b2Vec2 pointB(position.x + dir.x, position.y + dir.y);   // End of ray

    RayCastClosestCallback callback;
    world->RayCast(&callback, pointA, pointB);

    hitPos = glm::vec2(callback.Point.x, callback.Point.y);

    return callback.Hit;
}

bool AiController::IsHitUp(const b2World *world, glm::vec2 &hitPos) const {
    const auto center = GetCenter();

    if (IsHitDir(world,
                 center + glm::vec2{_halfCharacterSize.x, _fourthPartCharacterSize.y},
                 glm::vec2{0.0f, _fourthPartCharacterSize.y}, hitPos))
        return true;

    if (IsHitDir(world,
                 center + glm::vec2{-_halfCharacterSize.x, _fourthPartCharacterSize.y},
                 glm::vec2{0.0f, _fourthPartCharacterSize.y}, hitPos))
        return true;


    return false;
}

bool AiController::IsHitLeft(const b2World *world, glm::vec2 &hitPos) const {
    const auto center = GetCenter();

    if (IsHitDir(world,
                 center + glm::vec2(-_fourthPartCharacterSize.x, -_fourthPartCharacterSize.y),
                 glm::vec2(-_fourthPartCharacterSize.y, 0.0f), hitPos))
        return true;

    if (IsHitDir(world,
                 center + glm::vec2(-_fourthPartCharacterSize.x, _fourthPartCharacterSize.y),
                 glm::vec2(-_fourthPartCharacterSize.y, 0.0f), hitPos))
        return true;


    return false;
}

bool AiController::IsHitRight(const b2World *world, glm::vec2 &hitPos) const {
    const auto center = GetCenter();

    if (IsHitDir(world,
                 center + glm::vec2(_fourthPartCharacterSize.x, -_fourthPartCharacterSize.y),
                 glm::vec2(_fourthPartCharacterSize.y, 0.0f), hitPos))
        return true;

    if (IsHitDir(world,
                 center + glm::vec2(_fourthPartCharacterSize.x, _fourthPartCharacterSize.y),
                 glm::vec2(_fourthPartCharacterSize.y, 0.0f), hitPos))
        return true;

    return false;
}

bool AiController::IsGrounded(const b2World *world, glm::vec2 &hitPos) const {
    const auto center = GetCenter();

    if (IsHitDir(world,
                 center + glm::vec2{_halfCharacterSize.x, -_fourthPartCharacterSize.y},
                 glm::vec2{0.0f, -_fourthPartCharacterSize.y}, hitPos))
        return true;

    if (IsHitDir(world,
                 center + glm::vec2{-_halfCharacterSize.x, -_fourthPartCharacterSize.y},
                 glm::vec2{0.0f, -_fourthPartCharacterSize.y}, hitPos))
        return true;


    return false;
}

void AiController::UpdateInternal(const float &deltaTime, InputSystem *input, Transform *transform,
    b2World *world) {
    //SetAnimation(0, "run", true);
}

AiController::AiController(const std::weak_ptr<Entity> &entity): Component(entity) {
    _transform = _entity.lock()->GetComponent<Transform>();
}

void AiController::SetLookAt(glm::vec2 &lookAt) {
    if (const auto& render = _renderer.lock()) {
        render->LookAt(lookAt);
    }
}

void AiController::SetSpineRenderer(const std::weak_ptr<SpineRenderer> &spineRenderer) noexcept {
    _renderer = spineRenderer;
    _renderer.lock()->SetAnimation(0, "death", true);
}

void AiController::Update(const float &deltaTime) {

    if (const auto input = _inputSystem.lock())
        if (const auto transform = _transform.lock())
            if (const auto world = _world.lock())
                if (const auto b2World = world->GetWorld().lock())
                    UpdateInternal(deltaTime, input.get(), transform.get(), b2World.get());

}
