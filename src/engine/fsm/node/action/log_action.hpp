#pragma once
#include "action.hpp"
#include <iostream>
#include <string>

struct LogAction final : public Action {
private:
    std::string _message;

public:
    explicit LogAction(std::string message)
        : _message(std::move(message)) {}

    void OnEnter() const override {
        std::cout << "[FSM Log] " << _message << std::endl;
    }

    void OnUpdate() const override {
    }

    void OnExit() const override {
    }
};
