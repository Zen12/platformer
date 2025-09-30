#pragma once
#include "../../entity/component_serialization.hpp"
#include <string>

struct SpineRenderComponentSerialization final : public ComponentSerialization
{
    std::string SpineGuid;
};