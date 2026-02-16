#pragma once

#include "ui_page_asset.hpp"
#include "yaml-cpp/node/node.h"
#include "guid_yaml.hpp"

namespace YAML {
    template <>
    struct convert<UiPageAsset>
    {
        static bool decode(const Node &node, UiPageAsset &rhs)
        {
            rhs.CssGuid = node["css"].as<Guid>();
            rhs.MaterialGuid = node["material"].as<Guid>();
            rhs.FontGuid = node["font"].as<Guid>();
            rhs.RmlGuid = node["rml"].as<Guid>();
            return true;
        }
    };
}
