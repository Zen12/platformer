#pragma once
#include "../../asset/factories/component_factory.hpp"
#include "particle_emitter_component.hpp"
#include "particle_emitter_component_serialization.hpp"

class ParticleEmitterComponentFactory final : public ComponentFactory<ParticleEmitterComponent, ParticleEmitterSerialization> {
protected:
    void FillComponent(const std::weak_ptr<ParticleEmitterComponent> &component, const ParticleEmitterSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                comp->SetMaterial(scene->GetMaterial(serialization.materialGuid));
                comp->SetData({
                    .Count = serialization.count,
                    .Duration = serialization.duration,
                    .StartVelocity = serialization.startVelocity,
                    .EndVelocity = serialization.endVelocity,
                    .StartScale = serialization.startScale,
                    .EndScale = serialization.endScale,
                    .StartColor = serialization.startColor,
                    .EndColor = serialization.endColor}
                );

                if (const auto renderPipeline = scene->GetRenderPipeline().lock()) {
                    renderPipeline->AddRenderer(comp);
                }
            }
        }
    }
};