#pragma once
#include <string>

class Condition {
public:
    virtual ~Condition() = default;

    std::string Type;
    std::string Guid;

    [[nodiscard]] virtual bool IsSuccess() = 0;
};
