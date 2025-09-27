#pragma once

#include "../../asset/serialization/serialization_component.hpp"

struct IdleCharacterSerialization final : public ComponentSerialization {
    std::string IdleAnimation;
};