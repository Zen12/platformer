#pragma once

#include "entity/component_serialization.hpp"

struct CameraComponentSerialization final : public ComponentSerialization
{
    float perspectiveAspect;
    float perspectiveNearPlane;
    float perspectiveFarPlane;
    float orthographicAspect;
    bool isPerspective;
    float yDepthFactor = 0.0f;  // Y-based depth offset for 2.5D sorting (0 = disabled)
};