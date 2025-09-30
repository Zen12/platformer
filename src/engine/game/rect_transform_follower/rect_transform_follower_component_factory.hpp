#pragma once
#include "../../entity/component_factory.hpp"
#include "rect_transform_follower_component.hpp"
#include "rect_transform_follower_component_serialization.hpp"

class RectTransformFollowerFactory final : public ComponentFactory<RectTransformFollower, RectTransformFollowerSerialization> {
protected:
    void FillComponent(const std::weak_ptr<RectTransformFollower> &component,
        const RectTransformFollowerSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto entity = _entity.lock()) {
                if (const auto comp = component.lock()) {
                    comp->SetTransform(entity->GetComponent<RectTransform>());
                    comp->SetCamera(scene->FindByTag("main_camera").lock()->GetComponent<CameraComponent>().lock());
                    comp->SetOffset(serialization.Offset);

                    if (serialization.UseCreator) {
                        if (const auto creator = entity->GetCreator(); !creator.empty()) {
                            const auto creatorEntity = scene->GetEntityById(creator);
                            if (const auto e = creatorEntity.lock()) {
                                comp->SetFollowTarget(e->GetComponent<Transform>());
                            }
                        }
                    } else {
                        const auto target = scene->GetEntityById(serialization.Target);
                        if (const auto e = target.lock()) {
                            comp->SetFollowTarget(e->GetComponent<Transform>());
                        }
                    }
                }
            }
        }
    }
};