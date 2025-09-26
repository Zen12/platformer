#pragma once

#include "camera_component_serialization.hpp"
#include "../asset/factories/component_factory.hpp"
#include "camera_component.hpp"
#include "../render/camera.hpp"

class CameraComponentFactory final : public ComponentFactory<CameraComponent, CameraComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<CameraComponent> &component, const CameraComponentSerialization &serialization) override {
        if (const auto camera = component.lock()) {
            if (const auto scene = _scene.lock()) {
                camera->SetCamera
                        (Camera{serialization.aspectPower, false, serialization.isPerspective, _scene.lock()->GetWindow()});
                camera->SetWindow(scene->GetWindow());
            }
        }
    }
};