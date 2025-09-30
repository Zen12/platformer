#pragma once

#include "../../entity/component_serialization.hpp"

struct CameraComponentSerialization final : public ComponentSerialization
{
    float aspectPower;
    bool isPerspective;
};