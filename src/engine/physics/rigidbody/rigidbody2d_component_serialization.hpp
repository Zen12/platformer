#pragma once
#include "../../asset/serialization/serialization_component.hpp"

struct Rigidbody2dSerialization final : public ComponentSerialization
{
    bool isDynamic = false;
};