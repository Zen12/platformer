#pragma once
#include "ui_page_action_serialization.hpp"
#include "yaml-cpp/node/node.h"
#include "../../system/guid_yaml.hpp"

namespace YAML {
    template <>
    struct convert<UIPageActionSerialization>
    {
        static bool decode(const Node &node, UIPageActionSerialization &rhs)
        {
            rhs.UiPageGuid = node["page_guid"].as<Guid>();
            return true;
        }
    };
}
