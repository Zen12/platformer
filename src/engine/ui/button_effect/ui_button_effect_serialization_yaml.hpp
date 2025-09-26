#pragma once

#include "ui_button_effect_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<UiButtonEffectSerialization>
    {
        static bool decode([[maybe_unused]]const Node &node, [[maybe_unused]] UiButtonEffectSerialization &rhs)
        {
            return true;
        }
    };
}