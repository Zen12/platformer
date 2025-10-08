#include "shoot_component.hpp"

#define CHARACTER_CONTROLLER_DEBUG 0

void ShootComponent::Update([[maybe_unused]] const float &deltaTime) {

}

ShootResult ShootComponent::Shoot(const glm::vec3 &lookAt) const {
    ShootResult shootResult;

    if (const auto world = _world.lock()) {
        if (const auto animation = _animation.lock()) {
            const auto startPosition = animation->GetBonePosition(_boneName);
            const auto dir = lookAt - startPosition;
            const auto result = world->RayCast(startPosition, startPosition + dir * _raycastDistance, 1);

            shootResult.StartPosition = startPosition;
            shootResult.Direction = dir;

#ifndef NDEBUG
#if CHARACTER_CONTROLLER_DEBUG
            DebugLines::AddLine(startPosition, lookAt);
#endif
#endif

            if (result.IsHit) {
                shootResult.IsHit = true;
                shootResult.Rigidbody = result.Rigidbody;
                shootResult.Point = result.Point;
                shootResult.Normal = result.Normal;
            }
        }
    }

    return shootResult;
}
