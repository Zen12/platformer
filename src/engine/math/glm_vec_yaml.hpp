#pragma once


namespace YAML
{
    template <>
    struct convert<glm::vec2>
    {
        static bool decode(const Node &node, glm::vec2 &rhs)
        {
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template <>
    struct convert<glm::vec3>
    {
        static bool decode(const Node &node, glm::vec3 &rhs)
        {
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template <>
    struct convert<glm::vec4>
    {
        static bool decode(const Node &node, glm::vec4 &rhs)
        {
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

}