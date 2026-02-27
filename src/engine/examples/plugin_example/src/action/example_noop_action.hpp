#pragma once
#include <string>
#include <iostream>
#include "node/action/action.hpp"

struct ExampleNoopAction final : public Action {
    std::string _label;

    explicit ExampleNoopAction(std::string label) : _label(std::move(label)) {}

    void OnEnter() const override {
        std::cout << "[ExamplePlugin] Enter: " << _label << std::endl;
    }

    void OnUpdate() const override {}

    void OnExit() const override {
        std::cout << "[ExamplePlugin] Exit: " << _label << std::endl;
    }
};
