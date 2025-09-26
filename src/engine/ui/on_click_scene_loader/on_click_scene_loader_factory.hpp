#pragma once

#include "on_click_scene_loader_serialization.hpp"
#include "../../asset/factories/component_factory.hpp"
#include "on_click_scene_loader.hpp"
#include "../button/button_component.hpp"

class OnClickSceneLoaderFactory final : public ComponentFactory<OnClickSceneLoader, OnClickSceneLoaderSerialization> {
protected:
    void FillComponent(const std::weak_ptr<OnClickSceneLoader> &component,
        const OnClickSceneLoaderSerialization &serialization) override {

        if (const auto entity = _entity.lock()) {
            if (const auto comp = component.lock()) {
                comp->SetScene(_scene);
                comp->SetButton(entity->GetComponent<UiButtonComponent>());
                comp->SetLoadScene(serialization.SceneGuid);
            }
        }
    }
};