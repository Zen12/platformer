#pragma once

#include <iostream>
#include <utility>

class ProjectAsset
{
    public:
        std::string Name;
        std::vector<std::string> Scenes;

        ProjectAsset(std::string  name, const std::vector<std::string>& scenes)
            : Name(std::move(name)), Scenes(scenes)
        {}
};