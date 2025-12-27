#version 300 es
precision mediump float;

out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D tex;
uniform int useTexture;

void main()
{
    vec4 color;
    if (useTexture == 1) {
        color = vertexColor * texture(tex, texCoord);
    } else {
        color = vertexColor;
    }

    if(color.a < 0.1)
        discard;

    FragColor = color;
}