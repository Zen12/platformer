#pragma once
#include <string>
#include "action_serialiazion.hpp"

class HealthCheckActionSerialization final : public ActionSerialization {
public:
    std::string TriggerName;

    ~HealthCheckActionSerialization() override = default;
};
