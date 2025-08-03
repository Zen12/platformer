#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aLightColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aDarkColor;

uniform mat4 uMatrix;

out vec4 lightColor;
out vec4 darkColor;
out vec2 texCoord;

void main() {
    lightColor = aLightColor;
    darkColor = aDarkColor;
    texCoord = aTexCoord;
    gl_Position = uMatrix * vec4(aPos, 0.0, 1.0);
}