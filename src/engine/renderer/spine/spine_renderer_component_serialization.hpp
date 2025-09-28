#pragma once
#include "../../asset/serialization/serialization_component.hpp"
#include <string>

struct SpineRenderComponentSerialization final : public ComponentSerialization
{
    std::string SpineGuid;
};