#pragma once

#include "../../entity/component_serialization.hpp"

struct IdleCharacterSerialization final : public ComponentSerialization {
    std::string IdleAnimation;
};