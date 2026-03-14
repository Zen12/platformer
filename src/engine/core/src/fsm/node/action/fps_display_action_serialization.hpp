#pragma once
#include <string>
#include "action_serialization.hpp"

class FpsDisplayActionSerialization final : public ActionSerialization {
public:
    std::string ElementId;

    ~FpsDisplayActionSerialization() override = default;
};
