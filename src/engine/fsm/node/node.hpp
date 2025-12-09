#pragma once
#include <any>
#include <variant>
#include <vector>

#include "../../renderer/ui/ui_page_action.hpp"
#include "../../renderer/ui/button_listener_action.hpp"
#include "../../scene/load_scene_action.hpp"

class UIManager;
class SceneManager;

class StateNode final {
public:
    using AllActionVariants = std::variant<UiPageAction, LoadSceneAction, ButtonListenerAction>;
private:
    std::vector<AllActionVariants> _states{};
public:
    const std::string Guid;


    StateNode(std::string guid, const std::vector<AllActionVariants> &states, const std::shared_ptr<UIManager> &uiManager, const std::shared_ptr<SceneManager> &sceneManager = nullptr)
        : _states(states), Guid(std::move(guid)) {
        // Set UIManager and SceneManager on action instances
        for (auto& stateVar : _states) {
            std::visit([&uiManager, &sceneManager](auto& state) {
                using T = std::decay_t<decltype(state)>;
                if constexpr (std::is_same_v<T, UiPageAction>) {
                    state.SetUIManager(uiManager);
                } else if constexpr (std::is_same_v<T, LoadSceneAction>) {
                    if (sceneManager) {
                        state.SetSceneManager(sceneManager);
                    }
                } else if constexpr (std::is_same_v<T, ButtonListenerAction>) {
                    state.SetUIManager(uiManager);
                }
            }, stateVar);
        }
    }

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