#pragma once
#include "../../asset/serialization/serialization_component.hpp"

struct HealthComponentSerialization final : public ComponentSerialization {
    float Health;
};