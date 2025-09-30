#pragma once

#include <yaml-cpp/yaml.h>
#include "meta_asset.hpp"

namespace YAML
{
    template <>
    struct convert<MetaAsset>
    {
        static bool decode(const Node &node, MetaAsset &rhs)
        {
            rhs.Guid = node["guid"].as<std::string>();
            rhs.Extension = node["extension"].as<std::string>();
            rhs.Type = node["type"].as<std::string>();

            return true;
        }
    };
}
