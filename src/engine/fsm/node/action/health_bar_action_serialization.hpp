#pragma once
#include <string>
#include "action_serialiazion.hpp"

class HealthBarActionSerialization final : public ActionSerialization {
public:
    std::string ElementId;

    ~HealthBarActionSerialization() override = default;
};
