#version 300 es

precision mediump float;

out vec4 FragColor;

in vec3 localPosition;

uniform vec3 center;
uniform vec3 color;
uniform float radius;

float easeInExpo(float t) {
    return t <= 0.0 ? 0.0 : pow(2.0, 10.0 * (t - 1.0));
}

float easeOutCubic(float t) {
    return 1.0 - pow(1.0 - t, 3.0);
}

void main()
{
    float dist = distance(localPosition, center);
    float transperent = dist / radius;
    transperent = easeOutCubic(transperent);
    FragColor = vec4(color.xyz, 1.0 - transperent);
}