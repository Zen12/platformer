#pragma once

#include "camera_component.hpp"

#include "../window/window_component.hpp"
#include "../transform/transform_component.hpp"
#include "../esc_core.hpp"
#include "entt/entity/registry.hpp"

class CameraSystem final : public ISystem {

     const decltype(std::declval<entt::registry>().view<WindowComponent>()) WindowView;
     const decltype(std::declval<entt::registry>().view<CameraComponentV2, TransformComponentV2>()) CameraView;

 public:

     CameraSystem(const decltype(std::declval<entt::registry>().view<WindowComponent>()) &window,
     const decltype(std::declval<entt::registry>().view<CameraComponentV2, TransformComponentV2>()) &camera
     ) : WindowView(window), CameraView(camera) {

     }

     [[nodiscard]] glm::mat4 GetProjection(const glm::u16vec2 &windowsSize, const CameraComponentSerialization &data) const noexcept {
         if (data.isPerspective)
         {
             const float ration = static_cast<float>(windowsSize.x) / static_cast<float>(windowsSize.y);
             return glm::perspective(glm::radians(data.aspectPower), ration, -0.1f, 100.0f);
         }
         else if (data.isUi)
         {
             const float width = static_cast<float>(windowsSize.x) / data.aspectPower;
             const float height = static_cast<float>(windowsSize.y) / data.aspectPower;

             return glm::ortho(0.0f, width, 0.0f, height, -1.0f, 100.0f);
         } else { // ortho
             const float width = static_cast<float>(windowsSize.x) / data.aspectPower;
             const float height = static_cast<float>(windowsSize.y) / data.aspectPower;

             const float halfWidth = width * 0.5f;
             const float halfHeight = height * 0.5f;

             return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 100.0f);
         }
     }

     [[nodiscard]] const glm::u16vec2& GetViewportSize() const noexcept {
         for (const auto &[_, window] : WindowView.each()) {
             return window.Size;
         }
         static glm::u16vec2 defaultViewportSize{1,1};
         return defaultViewportSize;
     }
     void OnTick() override {

         const auto size = GetViewportSize();
         for (const auto &[_, camera, transform] : CameraView.each()) {
             camera.View = transform.GetModel();
             camera.Projection = GetProjection(size, camera.cameraData);
         }
     }
 };
