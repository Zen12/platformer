#pragma once

#include <memory>
#include "../../renderer/spine/spine_data.hpp"

class SpineComponent final {
    public:
    std::unique_ptr<SpineData> Spine{};

    explicit SpineComponent(const SpineData& serialization)
        : Spine(std::make_unique<SpineData>(serialization)) {}
};
