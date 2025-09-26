#pragma once

namespace YAML
{
    template <>
    struct convert<HealthBarComponentSerialization>
    {
        static bool decode(const Node &node, HealthBarComponentSerialization &rhs)
        {
            rhs.UseCreator = node["use_creator"].as<bool>();
            return true;
        }
    };
}