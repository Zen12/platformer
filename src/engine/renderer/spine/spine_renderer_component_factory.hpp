#pragma once
#include "../../entity/component_factory.hpp"
#include "spine_renderer_component.hpp"
#include "spine_renderer_component_serialization.hpp"
#include "spine_asset_loader.hpp"

class SpineRendererFactory final : public ComponentFactory<SpineRenderer, SpineRenderComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<SpineRenderer> &component, const SpineRenderComponentSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                const auto spineAsset = scene->GetAssetManager().lock()->LoadAssetByGuid<SpineAsset>(serialization.SpineGuid);
                const auto sprite = scene->GetSprite(spineAsset.image);
                const auto spineData = scene->GetSpineData(serialization.SpineGuid, spineAsset);
                const auto meshRenderer = _entity.lock()->GetComponent<MeshRenderer>();
                meshRenderer.lock()->SetSprite(sprite);
                comp->SetSpine(spineData);
                comp->SetSpineScale(spineAsset.spineScale);
                comp->SetMeshRenderer(meshRenderer);
            }
        }
    }
};