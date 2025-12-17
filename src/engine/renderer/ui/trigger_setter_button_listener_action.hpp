#pragma once

#include "../../fsm/node/action/action.hpp"
#include "../ui_manager.hpp"
#include <unordered_map>
#include <string>

struct TriggerSetterButtonListenerAction final : public Action {

private:
    std::string _buttonId;
    std::string _triggerName;
    std::shared_ptr<UIManager> _manager;
    std::unordered_map<std::string, bool>& _triggers;
    mutable int _handlerId = -1;

public:
    TriggerSetterButtonListenerAction(std::string buttonId, std::string triggerName, std::shared_ptr<UIManager> manager, std::unordered_map<std::string, bool>& triggers)
        : _buttonId(std::move(buttonId)), _triggerName(std::move(triggerName)), _manager(std::move(manager)), _triggers(triggers) {}

    void OnEnter() const override {
        if (_manager && !_buttonId.empty() && !_triggerName.empty()) {
            _handlerId = _manager->SetButtonClickHandler(_buttonId, [&triggers = _triggers, triggerName = _triggerName]() {
                std::cout << triggerName << ": true" << std::endl;
                triggers[triggerName] = true;
            });
        }
    }

    void OnUpdate() const override {
        // No update logic needed for button listener
    }

    void OnExit() const override {
        if (_manager && _handlerId != -1) {
            _manager->RemoveButtonClickHandler(_handlerId);
            _handlerId = -1;
        }
    }
};
