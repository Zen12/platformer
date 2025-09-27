#pragma once

#include "../../asset/serialization/serialization_component.hpp"

struct TeamSerialization final : public ComponentSerialization {
    int Team;
};