#pragma once
#include "../../asset/serialization/serialization_component.hpp"


struct UiTextComponentSerialization final : public ComponentSerialization {
    std::string MaterialGUID;
    std::string Text;
    glm::vec3 Color;
    float FontSize;
};