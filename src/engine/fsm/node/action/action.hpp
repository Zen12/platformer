#pragma once
#include <functional>

class Action {

protected:
    std::function<void()> _onEnter;
    std::function<void()> _onUpdate;
    std::function<void()> _onExit;

public:

    Action(std::function<void()> onEnter, std::function<void()> onUpdate, std::function<void()> onExit)
        : _onEnter(std::move(onEnter)), _onUpdate(std::move(onUpdate)), _onExit(std::move(onExit)) {

    }

    // Default constructor for derived classes that override the methods
    Action() = default;

    virtual ~Action() = default;

    virtual void OnEnter() const {
        if (_onEnter) {
            _onEnter();
        }
    }

    virtual void OnUpdate() const {
        if (_onUpdate) {
            _onUpdate();
        }
    }

    virtual void OnExit() const {
        if (_onExit) {
            _onExit();
        }
    }
};