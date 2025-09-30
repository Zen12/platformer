#pragma once
#include "../../entity/component_serialization.hpp"

struct Rigidbody2dSerialization final : public ComponentSerialization
{
    bool isDynamic = false;
};