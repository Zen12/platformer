#pragma once

#include "../../entity/component_serialization.hpp"

struct CameraComponentSerialization final : public ComponentSerialization
{
    bool isUi = false;
    float aspectPower;
    bool isPerspective;
};