#pragma once

#include "../../fsm/node/action/action.hpp"
#include "../ui_manager.hpp"

struct UiPageAction final : public Action {

private:
    std::string _pageGuid;
    std::shared_ptr<UIManager> _manager;

public:
    UiPageAction() = default;

    void SetPageGuid(const std::string &pageGuid) {
        _pageGuid = pageGuid;
    }

    void SetUIManager(const std::shared_ptr<UIManager> &uiManager) {
        _manager = uiManager;
    }

    void OnEnter() const override {
        if (_manager) {
            _manager->LoadPage(_pageGuid);
        }
    }

    void OnUpdate() const override {
        if (_manager) {
            _manager->Update();
        }
    }

    void OnExit() const override {
        if (_manager) {
            _manager->UnLoadPage(_pageGuid);
        }
    }
};