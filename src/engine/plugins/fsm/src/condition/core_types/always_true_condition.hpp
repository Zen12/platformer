#pragma once
#include <string>

struct AlwaysTrueCondition final {
    std::string Type;
    std::string Guid;

    AlwaysTrueCondition() = default;

    [[nodiscard]] bool IsSuccess() const {
        return true;
    }
};
