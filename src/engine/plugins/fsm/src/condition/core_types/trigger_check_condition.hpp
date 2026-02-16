#pragma once
#include <string>
#include <memory>
#include <utility>

#include "../../trigger_board.hpp"

struct TriggerCheckCondition final {
    std::string Type;
    std::string Guid;

private:
    std::string _triggerName;
    std::shared_ptr<TriggerBoard> _triggerBoard;

public:
    explicit TriggerCheckCondition(std::string triggerName, std::shared_ptr<TriggerBoard> triggerBoard)
        : _triggerName(std::move(triggerName)), _triggerBoard(std::move(triggerBoard)) {
    }

    [[nodiscard]] bool IsSuccess() const {
        return _triggerBoard->IsTriggerSet(_triggerName);
    }
};
