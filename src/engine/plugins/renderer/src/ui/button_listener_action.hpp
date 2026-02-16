#pragma once

#include "node/action/action.hpp"
#include "../ui_manager.hpp"

#define DEBUG_BUTTON_LISTENER 0

#if DEBUG_BUTTON_LISTENER
#include <iostream>
#define BUTTON_LOG if(1) std::cout
#else
#define BUTTON_LOG if(0) std::cout
#endif

struct ButtonListenerAction final : public Action {

private:
    std::string _buttonId;
    std::shared_ptr<UIManager> _manager;
    mutable int _handlerId = -1;

public:
    ButtonListenerAction(std::string buttonId, std::shared_ptr<UIManager> manager)
        : _buttonId(std::move(buttonId)), _manager(std::move(manager)) {}

    void OnEnter() const override {
        if (_manager && !_buttonId.empty()) {
            _handlerId = _manager->SetButtonClickHandler(_buttonId, [buttonId = _buttonId]() {
                BUTTON_LOG << "Button clicked: " << buttonId << std::endl;
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
