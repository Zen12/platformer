#pragma once

#include "../../renderer/sprite/sprite_renderer_component_serialization.hpp"

class SpriteComponentV2 final {
public:
    const SpriteRenderComponentSerialization Data;

    explicit SpriteComponentV2(SpriteRenderComponentSerialization data)
        : Data(std::move(data))
    {}
};