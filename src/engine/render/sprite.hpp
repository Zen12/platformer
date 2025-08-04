#pragma once

#include <GL/glew.h>


class Sprite
{
    public:
        explicit Sprite(const uint32_t &textureId);

        void Bind() const noexcept;

    private:
        uint32_t _textureId{};
};