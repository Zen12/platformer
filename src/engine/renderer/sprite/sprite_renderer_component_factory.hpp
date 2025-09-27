#pragma once
#include "../../asset/factories/component_factory.hpp"
#include "sprite_renderer_component.hpp"
#include "sprite_renderer_component_serialization.hpp"

class SpriteRendererFactory final : public ComponentFactory<SpriteRenderer, SpriteRenderComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<SpriteRenderer> &component, const SpriteRenderComponentSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                const auto material = scene->GetMaterial(serialization.MaterialGuid);
                const auto sprite = scene->GetSprite(serialization.SpriteGuid);
                comp->SetMaterial(material);
                comp->SetSprite(sprite);
                scene->GetRenderPipeline().lock()->AddRenderer(component);
            }
        }
    }
};