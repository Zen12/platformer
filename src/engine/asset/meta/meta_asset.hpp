#pragma once
#include <string>
#include "guid.hpp"

struct MetaAsset
{
    Guid Guid{};
    std::string Extension{};
    std::string Type{};
    std::string Path{};
};
