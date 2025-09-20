#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D ourTexture;
uniform float fillAmount;
uniform vec4 tint;

void main()
{
    if (TexCoords.x > fillAmount)
        discard;

    vec4 texColor = texture(ourTexture, TexCoords) * tint;
    color = texColor;
}