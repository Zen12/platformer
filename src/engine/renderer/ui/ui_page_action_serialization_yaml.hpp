#pragma once
#include "ui_page_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<UIPageActionSerialization>
    {
        static bool decode(const Node &node, UIPageActionSerialization &rhs)
        {
            rhs.UiPageGuid = node["page_guid"].as<std::string>();
            return true;
        }
    };
}
