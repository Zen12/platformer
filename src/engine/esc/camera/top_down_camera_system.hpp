#pragma once
#include "top_down_camera_component.hpp"
#include "../transform/transform_component.hpp"
#include "../tag/tag_component.hpp"
#include "../navmesh_agent/navmesh_agent_component.hpp"
#include "../time/time_component.hpp"
#include "../esc_core.hpp"
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <algorithm>

class TopDownCameraSystem final : public ISystemView<TopDownCameraComponent, TransformComponentV2> {
private:
    using TypeTagTransform = decltype(std::declval<entt::registry>().view<TagComponent, TransformComponentV2>());
    using TypeTimeView = decltype(std::declval<entt::registry>().view<DeltaTimeComponent>());
    const TypeTagTransform _tagTransformView;
    const TypeTimeView _timeView;
    entt::registry& _registry;

public:
    TopDownCameraSystem(
        const TypeView& view,
        const TypeTagTransform& tagTransformView,
        const TypeTimeView& timeView,
        entt::registry& registry)
        : ISystemView(view), _tagTransformView(tagTransformView), _timeView(timeView), _registry(registry) {}

    void OnTick() override {
        // Get delta time
        float deltaTime = 0.0f;
        for (auto [entity, time] : _timeView.each()) {
            deltaTime = time.Delta;
        }

        for (auto [cameraEntity, topDown, cameraTransform] : View.each()) {
            if (topDown.GetTargetTag().empty()) continue;

            // Find target entity with matching tag
            glm::vec3 targetPos{0.0f};
            glm::vec3 velocity{0.0f};
            bool foundTarget = false;
            entt::entity targetEntity = entt::null;

            for (auto [entity, tag, transform] : _tagTransformView.each()) {
                if (tag.GetTag() == topDown.GetTargetTag()) {
                    targetPos = transform.GetPosition();
                    targetEntity = entity;
                    foundTarget = true;
                    break;
                }
            }

            if (!foundTarget) continue;

            // Get velocity from NavmeshAgentComponent if available
            if (_registry.all_of<NavmeshAgentComponent>(targetEntity)) {
                const auto& navAgent = _registry.get<NavmeshAgentComponent>(targetEntity);
                velocity = navAgent.CurrentVelocity;
            }

            // Calculate look-ahead position based on velocity
            float speed = glm::length(velocity);
            glm::vec3 lookAheadOffset{0.0f};
            if (speed > 0.1f) {
                glm::vec3 direction = glm::normalize(velocity);
                // Scale look-ahead by speed, clamped to max
                float lookAheadDist = glm::min(speed, topDown.GetMaxLookAhead());
                lookAheadOffset = direction * lookAheadDist;
            }

            // Calculate desired camera position
            glm::vec3 desiredPosition = targetPos + lookAheadOffset + topDown.GetOffsetPosition();

            // Initialize on first frame
            if (!topDown.Initialized) {
                topDown.CurrentCameraPosition = desiredPosition;
                topDown.Initialized = true;
            }

            // Smooth lerp towards desired position
            float t = std::min(1.0f, topDown.GetSmoothSpeed() * deltaTime);
            topDown.CurrentCameraPosition = glm::mix(topDown.CurrentCameraPosition, desiredPosition, t);

            cameraTransform.SetPosition(topDown.CurrentCameraPosition);
            cameraTransform.SetEulerRotation(topDown.GetOffsetRotation());
        }
    }
};
