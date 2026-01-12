#version 330 core
in vec4 FragPosLightSpace;
in vec3 WorldPos;
out vec4 FragColor;

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
    // 3D checker pattern based on world position (X, Y, Z)
    float scale = 2.0;
    float checkerX = floor(WorldPos.x * scale);
    float checkerY = floor(WorldPos.y * scale);
    float checkerZ = floor(WorldPos.z * scale);
    float checker = mod(checkerX + checkerY + checkerZ, 2.0);

    // Color changes based on Y height (0 to 5)
    float heightFactor = clamp(WorldPos.y / 5.0, 0.0, 1.0);
    heightFactor = smoothstep(0.0, 1.0, heightFactor);

    // Low = green, High = brown/tan
    vec3 lowColor1 = vec3(0.2, 0.6, 0.2);   // Dark green
    vec3 lowColor2 = vec3(0.4, 0.8, 0.3);   // Light green
    vec3 highColor1 = vec3(0.5, 0.35, 0.2); // Dark brown
    vec3 highColor2 = vec3(0.7, 0.5, 0.3);  // Light tan

    vec3 color1 = mix(lowColor1, highColor1, heightFactor);
    vec3 color2 = mix(lowColor2, highColor2, heightFactor);

    vec3 baseColor = mix(color1, color2, checker);

    float shadow = calcShadow(FragPosLightSpace);
    FragColor = vec4(baseColor * (1.0 - shadow), 1.0);
}
