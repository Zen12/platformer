#pragma once
#include "../../entity/component_serialization.hpp"

struct NavmeshAgentComponentSerialization final : public ComponentSerialization {
    float Radius = 0.5f;
    float MaxSpeed = 5.0f;
    bool Enabled = true;
};
