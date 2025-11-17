#pragma once

#include "../../entity/component_serialization.hpp"

struct CameraComponentSerialization final : public ComponentSerialization
{
    float perspectiveAspect;
    float perspectiveNearPlane;
    float perspectiveFarPlane;
    float orthographicAspect;
    bool isPerspective;
};