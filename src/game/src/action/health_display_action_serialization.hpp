#pragma once
#include <string>
#include "node/action/action_serialization.hpp"

class HealthDisplayActionSerialization final : public ActionSerialization {
public:
    std::string ElementId;

    ~HealthDisplayActionSerialization() override = default;
};
