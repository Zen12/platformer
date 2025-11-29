#pragma once

#include "ui_page_asset.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<UiPageAsset>
    {
        static bool decode(const Node &node, UiPageAsset &rhs)
        {
            rhs.CssGuid = node["css"].as<std::string>();
            rhs.MaterialGuid = node["material"].as<std::string>();
            rhs.FontGuid = node["font"].as<std::string>();
            rhs.RmlGuid = node["rml"].as<std::string>();
            return true;
        }
    };
}
