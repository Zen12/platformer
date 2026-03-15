#version 330 core
#define MAX_SPOT_LIGHTS 4

in vec4 FragPosLightSpace;
in vec3 WorldPos;
out vec4 FragColor;

uniform vec4 lineColor;
uniform sampler2D shadowMap;
uniform sampler2D spotShadowMaps[MAX_SPOT_LIGHTS];

uniform int numSpotLights;
uniform vec3 spotLightPos[MAX_SPOT_LIGHTS];
uniform vec3 spotLightDir[MAX_SPOT_LIGHTS];
uniform vec3 spotLightColor[MAX_SPOT_LIGHTS];
uniform float spotInnerCutoff[MAX_SPOT_LIGHTS];
uniform float spotOuterCutoff[MAX_SPOT_LIGHTS];
uniform float spotLightRange[MAX_SPOT_LIGHTS];
uniform float spotLightIntensity[MAX_SPOT_LIGHTS];
uniform mat4 spotLightVP[MAX_SPOT_LIGHTS];

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

float calcSpotShadow(int index, vec3 worldPos)
{
    vec4 fragPosSpotLightSpace = spotLightVP[index] * vec4(worldPos, 1.0);
    vec3 projCoords = fragPosSpotLightSpace.xyz / fragPosSpotLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 1.0;
    float currentDepth = projCoords.z;
    float bias = 0.002;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(spotShadowMaps[index], 0));
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(spotShadowMaps[index], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

float calcSpotLight(int index, vec3 worldPos)
{
    vec3 toLight = spotLightPos[index] - worldPos;
    float distance = length(toLight);
    if (distance > spotLightRange[index]) return 0.0;
    vec3 lightDir = normalize(toLight);
    float theta = dot(lightDir, normalize(-spotLightDir[index]));
    float epsilon = spotInnerCutoff[index] - spotOuterCutoff[index];
    float spotEffect = clamp((theta - spotOuterCutoff[index]) / epsilon, 0.0, 1.0);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    return spotEffect * attenuation * spotLightIntensity[index];
}

void main()
{
    float shadow = calcShadow(FragPosLightSpace);
    vec3 spotContribution = vec3(0.0);
    for (int i = 0; i < numSpotLights; ++i) {
        float spotFactor = calcSpotLight(i, WorldPos);
        float spotShadow = calcSpotShadow(i, WorldPos);
        spotFactor *= (1.0 - spotShadow);
        spotContribution += spotLightColor[i] * spotFactor;
    }
    vec3 lit = lineColor.rgb * (1.0 - shadow) + lineColor.rgb * spotContribution;
    FragColor = vec4(lit, lineColor.a);
}
