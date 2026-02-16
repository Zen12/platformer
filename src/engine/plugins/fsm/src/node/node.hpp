#pragma once
#include <vector>
#include <string>
#include "../all_action_types.hpp"

class StateNode final {
private:
    mutable std::vector<fsm::AllActionVariants> _actions{};
public:
    const std::string Guid;

    StateNode(std::string guid, std::vector<fsm::AllActionVariants> actions)
        : _actions(std::move(actions)), Guid(std::move(guid)) {}

    void EnterAll() const {
        for (auto& action : _actions) {
            std::visit([](auto& a) {
                a.OnEnter();
            }, action);
        }
    }

    void UpdateAll() const {
        for (auto& action : _actions) {
            std::visit([](auto& a) {
                a.OnUpdate();
            }, action);
        }
    }

    void ExitAll() const {
        for (auto& action : _actions) {
            std::visit([](auto& a) {
                a.OnExit();
            }, action);
        }
    }
};
