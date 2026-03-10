#pragma once
#include <string>
#include "node/action/action_serialiazion.hpp"

class HealthBarActionSerialization final : public ActionSerialization {
public:
    std::string ElementId;

    ~HealthBarActionSerialization() override = default;
};
