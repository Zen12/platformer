#version 330 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D tex;
uniform bool useTexture;

void main()
{
    vec4 color;
    if (useTexture) {
        color = vertexColor * texture(tex, texCoord);
    } else {
        color = vertexColor;
    }

    if (color.a < 0.1)
        discard;

    FragColor = color;
}
