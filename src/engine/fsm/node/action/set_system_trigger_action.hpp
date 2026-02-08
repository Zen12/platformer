#pragma once
#include "action.hpp"
#include "../../system_triggers.hpp"
#include "../../trigger_board.hpp"
#include <memory>

struct SetSystemTriggerAction final : public Action {
private:
    std::shared_ptr<TriggerBoard> _triggerBoard;
    SystemTriggers _triggerType;

public:
    SetSystemTriggerAction(SystemTriggers triggerType, std::shared_ptr<TriggerBoard> triggerBoard)
        : _triggerBoard(std::move(triggerBoard)), _triggerType(triggerType) {}

    void OnEnter() const override {
        _triggerBoard->SetSystemTrigger(_triggerType);
    }

    void OnUpdate() const override {
    }

    void OnExit() const override {
    }
};
