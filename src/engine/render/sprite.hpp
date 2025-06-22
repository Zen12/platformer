#pragma once

#include <string>
#include <iostream>


class Sprite
{
    public:
        explicit Sprite(const std::string& data);
        void Bind() const noexcept;

    private:
        int32_t _x{};
        int32_t _y{};
        int32_t _channels{};
        uint32_t _textureId{};
};