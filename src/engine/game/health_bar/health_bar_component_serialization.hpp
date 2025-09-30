#pragma once

#include "../../entity/component_serialization.hpp"

struct HealthBarComponentSerialization final : public ComponentSerialization {
    bool UseCreator;
};