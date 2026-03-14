#pragma once
#include <vector>
#include <string>
#include <memory>
#include "action/action.hpp"

class StateNode final {
private:
    std::vector<std::unique_ptr<Action>> _actions{};
public:
    const std::string Guid;

    StateNode(std::string guid, std::vector<std::unique_ptr<Action>> actions)
        : _actions(std::move(actions)), Guid(std::move(guid)) {}

    void EnterAll() const {
        for (auto& action : _actions) {
            action->OnEnter();
        }
    }

    void UpdateAll() const {
        for (auto& action : _actions) {
            action->OnUpdate();
        }
    }

    void ExitAll() const {
        for (auto& action : _actions) {
            action->OnExit();
        }
    }
};
