#pragma once

#include "../asset/serialization/serialization_component.hpp"

struct CameraComponentSerialization final : public ComponentSerialization
{
    float aspectPower;
    bool isPerspective;
};