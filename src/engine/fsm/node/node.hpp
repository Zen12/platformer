#pragma once
#include <any>
#include <variant>
#include <vector>

#include "../../renderer/ui/ui_page_action.hpp"
#include "../../renderer/ui/button_listener_action.hpp"
#include "../../renderer/ui/trigger_setter_button_listener_action.hpp"
#include "../../scene/load_scene_action.hpp"

class StateNode final {
public:
    using AllActionVariants = std::variant<UiPageAction, LoadSceneAction, ButtonListenerAction, TriggerSetterButtonListenerAction>;
private:
    std::vector<AllActionVariants> _states{};
public:
    const std::string Guid;

    StateNode(std::string guid, std::vector<AllActionVariants> states)
        : _states(std::move(states)), Guid(std::move(guid)) {}

    void EnterAll() const {
        for (auto& stateVar : _states) {
            std::visit([](auto& state) {
                state.OnEnter();
            }, stateVar);
        }
    }

    void UpdateAll() const {
        for (auto &stateVar: _states) {
            std::visit([](auto& state) {
                state.OnUpdate();
            }, stateVar);
        }
    }

    void ExitAll() const {
        for (auto& stateVar : _states) {
            std::visit([](auto& state) {
                state.OnExit();
            }, stateVar);
        }
    }

};