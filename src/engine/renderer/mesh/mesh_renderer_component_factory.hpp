#pragma once
#include "../../asset/factories/component_factory.hpp"
#include "mesh_renderer_component.hpp"
#include "mesh_renderer_component_serialization.hpp"

class MeshRendererFactory final : public ComponentFactory<MeshRenderer, MeshRendererComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<MeshRenderer> &component, const MeshRendererComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {

                comp->SetMaterial(scene->GetMaterial(serialization.MaterialGuid));
                scene->GetRenderPipeline().lock()->AddRenderer(component);
            }
        }
    }
};