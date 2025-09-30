#pragma once

namespace YAML {
    template <>
    struct convert<ProjectAsset>
    {
        static bool decode(const Node &node, ProjectAsset &rhs)
        {
            rhs.Name = node["name"].as<std::string>();
            rhs.Scenes = node["scenes"].as<std::vector<std::string>>();
            rhs.Resolution = node["resolution"].as<std::vector<int>>();
            rhs.TargetFps = node["target_fps"].as<float>();
            return true;
        }
    };
}