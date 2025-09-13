#version 330 core
out vec4 FragColor;

in vec3 localPosition;
uniform vec3 center;
uniform vec3 color;
uniform float radius = 0.2;

void main()
{
    float dist = distance(localPosition, center);
    float transperent = 1 - dist / radius;
    transperent = clamp(transperent, 0.00, 1.0);
    FragColor = vec4(color.xyz, transperent);
}