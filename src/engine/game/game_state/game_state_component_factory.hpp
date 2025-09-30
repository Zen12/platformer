#pragma once

#include "game_state_component.hpp"
#include "game_state_component_serialization.hpp"
#include "../../entity/component_factory.hpp"

class GameStateFactory final : public ComponentFactory<GameStateController, GameStateData> {
protected:
    void FillComponent(const std::weak_ptr<GameStateController> &component,
        const GameStateData &serialization) override {

        if (const auto comp = component.lock()) {
            comp->SetLoseSceneId(serialization.LooseScene);
            comp->SetWinSceneId(serialization.WinScene);
            comp->SetScene(_scene);
        }
    }
};