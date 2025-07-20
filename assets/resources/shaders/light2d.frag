#version 330 core
out vec4 FragColor;

in vec3 localPosition;

void main()
{
    float dist = length(localPosition);
    FragColor = vec4(dist, 0, 0, 1 );
}