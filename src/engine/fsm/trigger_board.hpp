#pragma once

#include <string>
#include <unordered_map>
#include "system_triggers.hpp"

class TriggerBoard final {
private:
    std::unordered_map<std::string, bool> _triggers{};
    std::unordered_map<SystemTriggers, bool> _systemTriggers{};

public:
    TriggerBoard() = default;

    void SetTrigger(const std::string& triggerName) {
        _triggers[triggerName] = true;
    }

    [[nodiscard]] bool IsTriggerSet(const std::string& triggerName) const {
        auto it = _triggers.find(triggerName);
        return it != _triggers.end() && it->second;
    }

    void ClearTriggers() {
        _triggers.clear();
    }

    void SetSystemTrigger(SystemTriggers trigger) {
        _systemTriggers[trigger] = true;
    }

    [[nodiscard]] bool IsSystemTriggered(SystemTriggers trigger) const {
        auto it = _systemTriggers.find(trigger);
        return it != _systemTriggers.end() && it->second;
    }

    [[nodiscard]] std::unordered_map<std::string, bool>& GetTriggers() {
        return _triggers;
    }

    [[nodiscard]] std::unordered_map<SystemTriggers, bool>& GetSystemTriggers() {
        return _systemTriggers;
    }
};
