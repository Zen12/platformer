#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform int showDepth;

void main()
{
    if (showDepth == 1) {
        float depth = texture(depthTexture, TexCoord).r;
        FragColor = vec4(vec3(depth), 1.0);
    } else {
        FragColor = texture(screenTexture, TexCoord);
    }
}
