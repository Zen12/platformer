#pragma once

#include "../../fsm/node/action/action.hpp"
#include "guid.hpp"
#include "../ui_manager.hpp"

struct UiPageAction final : public Action {

private:
    Guid _pageGuid;
    std::shared_ptr<UIManager> _manager;

public:
    UiPageAction(Guid pageGuid, std::shared_ptr<UIManager> manager)
        : _pageGuid(std::move(pageGuid)), _manager(std::move(manager)) {}

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