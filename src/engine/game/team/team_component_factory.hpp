#pragma once

#include "team_component.hpp"
#include "team_component_serialization.hpp"
#include "../../entity/component_factory.hpp"
#include "../game_state/game_state_component.hpp"

class TeamComponentFactory final : public ComponentFactory<TeamComponent, TeamSerialization> {
protected:
    void FillComponent(const std::weak_ptr<TeamComponent> &component,
        const TeamSerialization &serialization) override
    {
        if (const auto &scene = _scene.lock()) {
            const auto gameStateEntity = scene->FindByTag("game_state"); //temp, need FSM

            if (const auto stateEntity = gameStateEntity.lock()) {
                if (const auto stateController = stateEntity->GetComponent<GameStateController>().lock()) {

                    if (const auto comp = component.lock()) {
                        comp->SetTeamRepository(stateController->GetTeamRepository());
                        comp->SetTeam(static_cast<Team>(serialization.Team));
                    }
                }
            }
        }
    }
};