#include "ai_controller_component.hpp"

#include "../../health/health_component.hpp"
#include "../../health_bar/health_bar_component.hpp"

#define DEBUG_AI_CONTROLLER 0

void AiController::SetAnimation(const size_t &index, const std::string &animation, const bool &isLoop) {

    if (_animationValue.find(index) != _animationValue.end()) {
        if (_animationValue[index] == animation) {
            return;
        }
    }

    _animationValue[index] = animation;
    if (const auto render = _renderer.lock()) {
        render->SetAnimation(index, animation, isLoop, false);
    }
}

void AiController::SetFaceRight(const bool &isFaceRight) const {
    if (const auto render = _renderer.lock()) {
        render->SetFaceRight(isFaceRight);
    }
}
bool AiController::TryToHit() {

    if (_hitCooldown > 0)
        return false;

    if (const auto target = _target.lock()) {
        if (const auto entity = target->GetEntity().lock()) {
            if (const auto health = entity->GetComponent<HealthComponent>().lock()) {
                health->DecreaseHealth(10);
                 _hitCooldown = 3;
            }
        }
    }
    return true;
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
    _renderer.lock()->SetAnimation(0,  _renderer.lock()->GetMoveAnimation(), true, false);
}

void AiController::Update(const float &deltaTime) {
    _hitCooldown-= deltaTime;

    if (const auto &movement = _characterMovementComponent.lock()) {
        if (const auto transform = _transform.lock()) {
            if (const auto render = _renderer.lock()) {
                if (const auto target = _target.lock()) {
                    if (const auto &finder = _astarFinder.lock()) {
                        if (const auto grip = _gridComponent.lock()) {

                            auto position = transform->GetPosition();
                            auto targetPos = target->GetPosition();
                            auto isTargetReachable = true;
                            const auto size = movement->GetCharacterSize();

                            const auto result = finder->GetPath(position + glm::vec3(0, size.y / 4, 0),
                                targetPos + glm::vec3(0,  size.y / 4, 0), false);

                            for (const auto &point: result) {
                                const auto pointPos = grip->GetPositionFromIndex(point.x, point.y);
                                if (const auto dif = glm::distance(position, pointPos); dif > 1) {
                                    targetPos = pointPos;
                                    isTargetReachable = false;
                                    break;
                                }
                            }
#ifndef NDEBUG
#if DEBUG_AI_CONTROLLER
                            auto previousPos = position;
                            for (const auto & point: result) {
                                const auto pointPos = grip->GetPositionFromIndex(point.x, point.y);
                                DebugLines::AddLine(previousPos, pointPos);
                                previousPos = pointPos;
                            }
#endif
#endif
                            if (result.empty())
                                isTargetReachable = false;

                            if (std::abs(targetPos.x - position.x) > 0.1f) {
                                if (targetPos.x < position.x) {
                                    SetFaceRight(false);
                                    movement->SetDirection(glm::vec3(-1, 0, 0));
                                } else  {
                                    movement->SetDirection(glm::vec3(1, 0, 0));
                                    SetFaceRight(true);
                                }
                            }

                            if (glm::distance(targetPos, position) < 1) {
                                if (isTargetReachable) {
                                    if (TryToHit()) {
                                        SetAnimation(0, render->GetHitAnimation(), true);
                                    }
                                }
                            }  else {
                                SetAnimation(0,  render->GetMoveAnimation(), true);
                            }
                        }
                    }
                }
            }
        }
    }
}
