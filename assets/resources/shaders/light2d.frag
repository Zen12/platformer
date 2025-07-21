#version 330 core
out vec4 FragColor;

in vec3 localPosition;
uniform vec3 center;

void main()
{
    float dist = distance(localPosition, center);
    float transperent = 1.0 - (dist * 0.1);
    transperent = clamp(transperent, 0.01, 0.99) + 0.01; // a bit of light for far-far distance
    FragColor = vec4(1, 1, 0, transperent);
}