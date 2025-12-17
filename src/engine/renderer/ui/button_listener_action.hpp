#pragma once

#include "../../fsm/node/action/action.hpp"
#include "../ui_manager.hpp"
#include <iostream>

struct ButtonListenerAction final : public Action {

private:
    std::string _buttonId;
    std::shared_ptr<UIManager> _manager;

public:
    ButtonListenerAction(std::string buttonId, std::shared_ptr<UIManager> manager)
        : _buttonId(std::move(buttonId)), _manager(std::move(manager)) {}

    void OnEnter() const override {
        if (_manager && !_buttonId.empty()) {
            _manager->SetButtonClickHandler(_buttonId, [buttonId = _buttonId]() {
                std::cout << "Button clicked: " << buttonId << std::endl;
            });
        }
    }

    void OnUpdate() const override {
        // No update logic needed for button listener
    }

    void OnExit() const override {
        // Cleanup will be handled by UIManager::Destroy()
    }
};
