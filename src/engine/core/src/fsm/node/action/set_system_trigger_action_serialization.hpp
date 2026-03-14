#pragma once
#include <string>
#include "action_serialization.hpp"

class SetSystemTriggerActionSerialization final : public ActionSerialization {
public:
    std::string TriggerType;

    ~SetSystemTriggerActionSerialization() override = default;
};
