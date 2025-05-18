#pragma once

#include <iostream>

class ProjectAsset
{
    public:
        std::string Name;
        std::vector<std::string> Scenes;

        ProjectAsset(const std::string& name, const std::vector<std::string>& scenes)
            : Name(name), Scenes(scenes)
        {}
};