#include "font.hpp"
#include <GL/glew.h>

Font::~Font()
{
    for (auto& [ch, character] : Characters)
    {
        glDeleteTextures(1, &character.TextureID);
    }
}

Font::Font(Font&& other) noexcept
    : Characters(std::move(other.Characters))
{
    other.Characters.clear();
}

Font& Font::operator=(Font&& other) noexcept
{
    if (this != &other)
    {
        for (auto& [ch, character] : Characters)
        {
            glDeleteTextures(1, &character.TextureID);
        }

        Characters = std::move(other.Characters);
        other.Characters.clear();
    }
    return *this;
}
