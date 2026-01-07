#pragma once
#include "../../entity/component_serialization.hpp"

struct HealthComponentSerialization final : public ComponentSerialization {
    float MaxHealth = 100.0f;
};
