#version 300 es
precision mediump float;
in vec3 WorldPos;
out vec4 FragColor;

void main()
{
    // Create checker pattern based on world position
    float scale = 0.5;
    float checker = mod(floor(WorldPos.x / scale) + floor(WorldPos.y / scale) + floor(WorldPos.z / scale), 2.0);

    vec3 color1 = vec3(0.9, 0.9, 0.9);  // Light gray
    vec3 color2 = vec3(0.3, 0.3, 0.3);  // Dark gray

    vec3 finalColor = mix(color1, color2, checker);
    FragColor = vec4(finalColor, 1.0);
}
