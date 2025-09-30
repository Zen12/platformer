#pragma once
#include "../../entity/component_factory.hpp"
#include "light_2d_component.hpp"
#include "light_2d_component_serialization.hpp"

class Light2dFactory final : public ComponentFactory<Light2dComponent, Light2dComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<Light2dComponent> &component, const Light2dComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {

                const auto ref = scene->GetEntity(serialization.CenterTransform);
                if (const auto transform = ref->GetComponent<Transform>().lock()) {
                    comp->SetCenterTransform(transform);
                }

                comp->SetPhysicsWorld(scene->GetPhysicsWorld());
                comp->SetMeshRenderer(_entity.lock()->GetComponent<MeshRenderer>());
                comp->SetSettings(Light2dSettings{
                    .Radius = serialization.Radius,
                    .Segments = serialization.Segments,
                    .Color = serialization.Color,
                    .Offset = serialization.Offset,
                    .MaxAngle = serialization.MaxAngle,
                    .StartAngle = serialization.StartAngle,
                });
            }
        }
    }
};