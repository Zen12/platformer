#version 330 core
in vec3 TexCoords;
out vec4 FragColor;

uniform sampler2D skyboxTexture;

const float PI = 3.14159265359;

void main()
{
    vec3 dir = normalize(TexCoords);

    float theta = atan(dir.z, dir.x);
    float phi = asin(clamp(dir.y, -1.0, 1.0));

    vec2 uv;
    uv.x = (theta + PI) / (2.0 * PI);
    uv.y = (phi + PI / 2.0) / PI;

    FragColor = texture(skyboxTexture, uv);
}
