#pragma once

#include "../../entity/component_serialization.hpp"

struct TeamSerialization final : public ComponentSerialization {
    int Team;
};