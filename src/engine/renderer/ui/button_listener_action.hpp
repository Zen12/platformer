#pragma once

#include "../../fsm/node/action/action.hpp"
#include "../ui_manager.hpp"
#include <iostream>

struct ButtonListenerAction final : public Action {

private:
    std::string _buttonId;
    std::shared_ptr<UIManager> _manager;

public:
    ButtonListenerAction() = default;

    void SetButtonId(const std::string &buttonId) {
        _buttonId = buttonId;
    }

    void SetUIManager(const std::shared_ptr<UIManager> &uiManager) {
        _manager = uiManager;
    }

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
