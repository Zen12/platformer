#pragma once
#include <string>
#include "../../system/guid.hpp"

struct MetaAsset
{
    Guid Guid{};
    std::string Extension{};
    std::string Type{};
    std::string Path{};
};
