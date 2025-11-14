#pragma once

#include <GL/glew.h>


class Texture
{
    public:
        explicit Texture(const uint32_t &textureId);
        ~Texture();

        void Bind() const noexcept;
        uint32_t GetTextureId() const noexcept;

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&&) noexcept;
        Texture& operator=(Texture&&) noexcept;

    private:
        uint32_t _textureId{};
};