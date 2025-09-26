#pragma once

#include "show_fps_component_serialization.hpp"
#include "../../asset/factories/component_factory.hpp"
#include "show_fps_component.hpp"
#include "../text/text_renderer_component.hpp"

class ShowFpsComponentFactory final : public ComponentFactory<ShowFpsComponent, ShowFpsComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<ShowFpsComponent> &component, [[maybe_unused]] const ShowFpsComponentSerialization &serialization) override {
        if (const auto entity = _entity.lock()) {
            if (const auto comp = component.lock()) {
                comp->SetText(entity->GetComponent<UiTextRendererComponent>());
            }
        }
    }
};