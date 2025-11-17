#pragma once
#include <algorithm>

#include "camera_component_serialization.hpp"
#include "glm/mat4x4.hpp"

class CameraComponentV2 final
{
public:
    const CameraComponentSerialization cameraData{};
    glm::mat4 Projection{};
    glm::mat4 View{};

    explicit CameraComponentV2(CameraComponentSerialization data)
        : cameraData(std::move(data))
    {}

};
