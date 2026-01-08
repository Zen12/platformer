#version 300 es
precision mediump float;

in vec2 TexCoord;
in vec4 FragPosLightSpace;
in vec4 InstanceColor;
out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D shadowMap;

float calcShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = 0.001;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));

    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 25.0;

    return shadow * 0.5;
}

void main()
{
    vec4 texColor = texture(texture1, TexCoord);
    // Use texture alpha with instance color for tinting
    vec3 tintedRgb = texColor.rgb * InstanceColor.rgb;
    float tintedAlpha = texColor.a * InstanceColor.a;
    float shadow = calcShadow(FragPosLightSpace);
    FragColor = vec4(tintedRgb * (1.0 - shadow), tintedAlpha);
}
