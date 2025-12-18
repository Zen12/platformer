#pragma once
#include "action.hpp"
#include "../../system_triggers.hpp"
#include <unordered_map>

struct SetSystemTriggerAction final : public Action {
private:
    std::unordered_map<SystemTriggers, bool>& _systemTriggers;
    SystemTriggers _triggerType;

public:
    SetSystemTriggerAction(SystemTriggers triggerType, std::unordered_map<SystemTriggers, bool>& systemTriggers)
        : _triggerType(triggerType), _systemTriggers(systemTriggers) {}

    void OnEnter() const override {
        _systemTriggers[_triggerType] = true;
    }

    void OnUpdate() const override {
    }

    void OnExit() const override {
    }
};
