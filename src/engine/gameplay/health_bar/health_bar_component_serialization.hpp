#pragma once

#include "../../asset/serialization/serialization_component.hpp"

struct HealthBarComponentSerialization final : public ComponentSerialization {
    bool UseCreator;
};