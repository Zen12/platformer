#pragma once
#include <string>
#include "node/action/action_serialiazion.hpp"

class HealthDisplayActionSerialization final : public ActionSerialization {
public:
    std::string ElementId;

    ~HealthDisplayActionSerialization() override = default;
};
