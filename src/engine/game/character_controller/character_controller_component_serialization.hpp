#pragma once
#include "../../entity/component_serialization.hpp"

struct CharacterControllerComponentSerialization final : public ComponentSerialization
{
    float Damage{10};
};