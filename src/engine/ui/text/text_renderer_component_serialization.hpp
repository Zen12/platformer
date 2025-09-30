#pragma once
#include "../../entity/component_serialization.hpp"


struct UiTextComponentSerialization final : public ComponentSerialization {
    std::string MaterialGUID;
    std::string Text;
    glm::vec3 Color;
    float FontSize;
};