#pragma once

#include "node/action/action.hpp"
#include "trigger_board.hpp"
#include "../ui_manager.hpp"
#include <string>

struct TriggerSetterButtonListenerAction final : public Action {

private:
    std::string _buttonId;
    std::string _triggerName;
    std::shared_ptr<UIManager> _manager;
    std::shared_ptr<TriggerBoard> _triggerBoard;
    mutable int _handlerId = -1;

public:
    TriggerSetterButtonListenerAction(std::string buttonId, std::string triggerName, std::shared_ptr<UIManager> manager, std::shared_ptr<TriggerBoard> triggerBoard)
        : _buttonId(std::move(buttonId)), _triggerName(std::move(triggerName)), _manager(std::move(manager)), _triggerBoard(std::move(triggerBoard)) {}

    void OnEnter() const override {
        if (_manager && !_buttonId.empty() && !_triggerName.empty()) {
            _handlerId = _manager->SetButtonClickHandler(_buttonId, [triggerBoard = _triggerBoard, triggerName = _triggerName]() {
                triggerBoard->SetTrigger(triggerName);
            });
        }
    }

    void OnUpdate() const override {
    }

    void OnExit() const override {
        if (_manager && _handlerId != -1) {
            _manager->RemoveButtonClickHandler(_handlerId);
            _handlerId = -1;
        }
    }
};
