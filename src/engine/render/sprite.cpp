#include "sprite.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glew.h>


Sprite::Sprite(const std::string &path)
{
    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true); 
    unsigned char *data = stbi_load(path.c_str(), &_x, &_y, &_channels, 0);

    if (data == nullptr)
    {
        std::cerr << "Failed to load texture" << std::endl;
        return;
    }

    switch (_channels)
    {
        case 3:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _x, _y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            break;
        case 4:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _x, _y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            break;
        default:
            std::cerr << "No support for channel: " << _channels << std::endl;
            break;
    }
            
    stbi_image_free(data);
    
}

void Sprite::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, _textureId);
}
