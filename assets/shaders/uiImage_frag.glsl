#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D ourTexture;

void main()
{    
    vec4 texColor = texture(ourTexture, TexCoords);
    color = texColor;
}