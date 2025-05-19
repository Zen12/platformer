#pragma once

#include <string>
#include <iostream>


class Sprite
{
    public:
        Sprite(const std::string& data);
        void Bind() const;

    private:
        int32_t _x;
        int32_t _y;
        int32_t _channels;
        uint32_t _textureId;
};