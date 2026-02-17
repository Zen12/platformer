#pragma once
#include <string>

class ConditionBase {
public:
    std::string Type;
    std::string Guid;

    virtual ~ConditionBase() = default;

    [[nodiscard]] virtual bool IsSuccess() const = 0;
};
