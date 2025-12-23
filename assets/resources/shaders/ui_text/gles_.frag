#version 300 es
precision mediump float;

out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D tex;
uniform int useTexture;

void main()
{
    if (useTexture == 1) {
        FragColor = vertexColor * texture(tex, texCoord);
    } else {
        FragColor = vertexColor;
    }
}