#version 300 es
precision highp float;

in vec3 WorldPos;
out vec4 FragColor;

uniform sampler2D sceneDepth;
uniform mat4 inverseVP;
uniform vec2 screenSize;

void main()
{
    vec2 screenUV = gl_FragCoord.xy / screenSize;
    float depth = texture(sceneDepth, screenUV).r;

    vec4 ndcPos = vec4(screenUV * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 worldPosFromDepth = inverseVP * ndcPos;
    worldPosFromDepth /= worldPosFromDepth.w;

    float fogPlaneY = WorldPos.y;
    float belowFog = max(0.0, fogPlaneY - worldPosFromDepth.y);

    float fogDensity = 0.3;
    float fogFactor = 1.0 - exp(-fogDensity * belowFog);

    vec3 fogColor = vec3(0.7, 0.8, 0.9);

    FragColor = vec4(fogColor, fogFactor);
}
