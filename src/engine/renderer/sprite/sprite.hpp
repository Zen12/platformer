#pragma once

#include <GL/glew.h>


class Sprite
{
    public:
        explicit Sprite(const uint32_t &textureId);
        ~Sprite();

        void Bind() const noexcept;
        uint32_t GetTextureId() const noexcept;

        Sprite(const Sprite&) = delete;
        Sprite& operator=(const Sprite&) = delete;

        Sprite(Sprite&&) noexcept;
        Sprite& operator=(Sprite&&) noexcept;

    private:
        uint32_t _textureId{};
};