#include <string>
#include <iostream>


class Sprite
{
    public:
        Sprite(const std::string& data);
        void Bind();

    private:
        int32_t x;
        int32_t y;
        int32_t channels;
        uint32_t textureId;
};