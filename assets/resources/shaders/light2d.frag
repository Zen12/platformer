#version 330 core
out vec4 FragColor;

in vec3 localPosition;
uniform vec3 center;

void main()
{
    float dist = distance(localPosition, center);
    float alpha = (1 - dist / 7) - 0.3;
    FragColor = vec4(1.0,1.0, 0, alpha);
}