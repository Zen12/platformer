#pragma once
#include "../../entity/component_serialization.hpp"

struct CameraControllerComponentSerialization final : public ComponentSerialization {
    float MoveSpeed = 10.0f;
    float MouseSensitivity = 0.1f;

    ~CameraControllerComponentSerialization() override = default;
};
