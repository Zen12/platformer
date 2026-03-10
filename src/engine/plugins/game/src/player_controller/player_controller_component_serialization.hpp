#pragma once
#include "entity/component_serialization.hpp"

struct PlayerControllerComponentSerialization final : public ComponentSerialization {
    float MoveSpeed = 5.0f;
    float DestinationDistance = 1.0f;
};
