#pragma once
#include <string>
#include "action_serialiazion.hpp"

class HealthDisplayActionSerialization final : public ActionSerialization {
public:
    std::string ElementId;

    ~HealthDisplayActionSerialization() override = default;
};
