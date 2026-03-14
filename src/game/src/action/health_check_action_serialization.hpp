#pragma once
#include <string>
#include "node/action/action_serialization.hpp"

class HealthCheckActionSerialization final : public ActionSerialization {
public:
    std::string TriggerName;

    ~HealthCheckActionSerialization() override = default;
};
