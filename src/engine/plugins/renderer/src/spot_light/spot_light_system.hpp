#pragma once

#include "spot_light_component.hpp"
#include "transform/transform_component.hpp"
#include "esc/esc_core.hpp"
#include "buffer/render_buffer.hpp"
#include "buffer/render_buffer_component.hpp"
#include "camera/camera_component.hpp"
#include "frustum.hpp"
#include "entt/entity/registry.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>
#include <algorithm>

class SpotLightSystem final : public ISystem {
    const decltype(std::declval<entt::registry>().view<SpotLightComponent, TransformComponentV2>()) _lightView;
    const decltype(std::declval<entt::registry>().view<CameraComponentV2>()) _cameraView;

    using TypeRenderBuffer = decltype(std::declval<entt::registry>().view<RenderBufferComponent>());
    const TypeRenderBuffer _renderBufferView;

    struct LightCandidate {
        float DistanceSq;
        glm::vec3 Position;
        glm::vec3 Direction;
        glm::vec3 Color;
        float InnerCutoff;
        float OuterCutoff;
        float Range;
        float Intensity;
        glm::mat4 View;
        glm::mat4 Projection;
    };

public:
    SpotLightSystem(
        const decltype(std::declval<entt::registry>().view<SpotLightComponent, TransformComponentV2>()) &lightView,
        const decltype(std::declval<entt::registry>().view<CameraComponentV2>()) &cameraView,
        const TypeRenderBuffer& renderBufferView)
        : _lightView(lightView), _cameraView(cameraView), _renderBufferView(renderBufferView) {}

    void OnTick() override {
        std::shared_ptr<RenderBuffer> renderBuffer;
        for (const auto& [_, rb] : _renderBufferView.each()) { renderBuffer = rb.Buffer; }
        if (!renderBuffer) return;

        Frustum frustum;
        glm::vec3 cameraPos{0.0f};
        for (const auto& [_, camera] : _cameraView.each()) {
            frustum.ExtractPlanes(camera.Projection * camera.View);
            cameraPos = glm::vec3(glm::inverse(camera.View)[3]);
        }

        std::vector<LightCandidate> candidates;

        for (const auto &[_, light, transform] : _lightView.each()) {
            const glm::vec3& position = transform.GetPosition();
            const glm::vec3& euler = transform.GetEulerRotation();

            if (!frustum.IsSphereVisible(position, light.LightData.Range)) {
                continue;
            }

            const glm::mat4 RX = glm::rotate(glm::mat4(1.0f), glm::radians(euler.x), glm::vec3(1,0,0));
            const glm::mat4 RY = glm::rotate(glm::mat4(1.0f), glm::radians(euler.y), glm::vec3(0,1,0));
            const glm::mat4 RZ = glm::rotate(glm::mat4(1.0f), glm::radians(euler.z), glm::vec3(0,0,1));
            const glm::mat4 rotationMatrix = RZ * RY * RX;

            const glm::vec3 direction = glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));

            const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
            const glm::mat4 modelMatrix = translationMatrix * rotationMatrix;
            const glm::mat4 viewMatrix = glm::inverse(modelMatrix);

            const glm::mat4 projMatrix = glm::perspective(
                2.0f * glm::radians(light.LightData.OuterAngle),
                1.0f,
                light.LightData.NearPlane,
                light.LightData.FarPlane
            );

            const float innerCutoff = std::cos(glm::radians(light.LightData.InnerAngle));
            const float outerCutoff = std::cos(glm::radians(light.LightData.OuterAngle));

            const glm::vec3 diff = position - cameraPos;
            const float distSq = glm::dot(diff, diff);

            candidates.push_back({distSq, position, direction, light.LightData.Color,
                                   innerCutoff, outerCutoff, light.LightData.Range, light.LightData.Intensity,
                                   viewMatrix, projMatrix});
        }

        std::sort(candidates.begin(), candidates.end(),
            [](const LightCandidate& a, const LightCandidate& b) { return a.DistanceSq < b.DistanceSq; });

        const int count = std::min(static_cast<int>(candidates.size()), MAX_SPOT_LIGHTS);
        for (int i = 0; i < count; i++) {
            const auto& c = candidates[i];
            renderBuffer->AddSpotLight(c.Position, c.Direction, c.Color,
                                        c.InnerCutoff, c.OuterCutoff, c.Range, c.Intensity,
                                        c.View, c.Projection);
        }
    }
};
