#pragma once
#include <unordered_map>
#include <string>
#include <utility>

#include "../condition.hpp"


class TriggerCheckCondition final : public Condition {

    const std::string _triggerName;
    const std::unordered_map<std::string, bool>& _triggers;


public:

    explicit TriggerCheckCondition(std::string  triggerName, const std::unordered_map<std::string, bool>& triggers)
        : _triggerName(std::move(triggerName)), _triggers(triggers) {
    }

    [[nodiscard]] bool IsSuccess() override {

        const auto it = _triggers.find(_triggerName);
        if (it == _triggers.end()) {
            return false; // Trigger not found, treat as false
        }

        return it->second; // Check if trigger is true
    }


};
