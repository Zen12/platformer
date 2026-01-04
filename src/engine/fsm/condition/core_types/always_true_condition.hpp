#pragma once
#include "../condition.hpp"

class AlwaysTrueCondition final : public Condition {
public:
    AlwaysTrueCondition() = default;

    [[nodiscard]] bool IsSuccess() override {
        return true;
    }
};
