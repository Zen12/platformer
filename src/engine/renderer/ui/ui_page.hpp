#pragma once
#include <string>

#include "../material/material.hpp"

class UiPage final {
public:
    std::string Rml;
    std::string Css;
    std::shared_ptr<Material> Material;
    std::string FontPath;
};
