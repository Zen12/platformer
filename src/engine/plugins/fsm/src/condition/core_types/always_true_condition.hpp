#pragma once
#include <string>

#include "../condition_base.hpp"

struct AlwaysTrueCondition final : public ConditionBase {
    AlwaysTrueCondition() = default;

    [[nodiscard]] bool IsSuccess() const override {
        return true;
    }
};
