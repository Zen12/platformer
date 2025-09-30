#pragma once
#include "../../entity/component_serialization.hpp"

struct HealthComponentSerialization final : public ComponentSerialization {
    float Health;
};