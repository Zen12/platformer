#pragma once

#include "directional_light_component.hpp"
#include "transform/transform_component.hpp"
#include "tag/tag_component.hpp"
#include "esc/esc_core.hpp"
#include "render_repository.hpp"
#include "entt/entity/registry.hpp"
#include <glm/gtc/matrix_transform.hpp>

class DirectionalLightSystem final : public ISystem {
    const decltype(std::declval<entt::registry>().view<DirectionalLightComponent, TransformComponentV2>()) LightView;
    const decltype(std::declval<entt::registry>().view<TagComponent, TransformComponentV2>()) TagView;
    std::shared_ptr<RenderRepository> _renderRepository;

public:
    DirectionalLightSystem(
        const decltype(std::declval<entt::registry>().view<DirectionalLightComponent, TransformComponentV2>()) &lightView,
        const decltype(std::declval<entt::registry>().view<TagComponent, TransformComponentV2>()) &tagView,
        std::shared_ptr<RenderRepository> renderRepository)
        : LightView(lightView), TagView(tagView), _renderRepository(std::move(renderRepository)) {}

    void OnTick() override {
        for (const auto &[_, light, transform] : LightView.each()) {
            glm::vec3 lightPosition = transform.GetPosition();

            if (!light.LightData.FollowTag.empty()) {
                for (const auto &[entity, tag, targetTransform] : TagView.each()) {
                    if (tag.GetTag() == light.LightData.FollowTag) {
                        lightPosition = targetTransform.GetPosition() + light.LightData.FollowOffset;
                        break;
                    }
                }
            }

            const glm::vec3& euler = transform.GetEulerRotation();
            const glm::mat4 RX = glm::rotate(glm::mat4(1.0f), glm::radians(euler.x), glm::vec3(1,0,0));
            const glm::mat4 RY = glm::rotate(glm::mat4(1.0f), glm::radians(euler.y), glm::vec3(0,1,0));
            const glm::mat4 RZ = glm::rotate(glm::mat4(1.0f), glm::radians(euler.z), glm::vec3(0,0,1));
            glm::mat4 rotationMatrix = RZ * RY * RX;
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), lightPosition);
            glm::mat4 modelMatrix = translationMatrix * rotationMatrix;
            light.View = glm::inverse(modelMatrix);

            const float halfSize = light.LightData.OrthoSize * 0.5f;
            const float halfWidth = halfSize * light.LightData.AspectRatio;
            const float halfHeight = halfSize;

            light.Projection = glm::ortho(
                -halfWidth, halfWidth,
                -halfHeight, halfHeight,
                light.LightData.NearPlane, light.LightData.FarPlane
            );

            if (_renderRepository) {
                _renderRepository->SetDirectionalLight(light.View, light.Projection);
            }
        }
    }
};
