#pragma once

#include "button_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<UiButtonComponentSerialization>
    {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]]  UiButtonComponentSerialization &rhs)
        {
            return true;
        }
    };
}