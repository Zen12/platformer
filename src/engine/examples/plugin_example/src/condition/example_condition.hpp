#pragma once
#include <string>
#include "condition/condition_base.hpp"

// Example FSM condition that always returns true
// Usage in .fsm: type: example_always_pass
struct ExampleAlwaysPassCondition final : public ConditionBase {
    [[nodiscard]] bool IsSuccess() const override {
        return true;
    }
};
