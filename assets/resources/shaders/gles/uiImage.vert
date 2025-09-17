#version 300 es
layout(location = 0) in vec3 vertex;     // position
layout(location = 1) in vec2 aTexCoord;  // texture coordinates

out vec2 TexCoords;  // pass to fragment shader

uniform mat4 projection;
uniform mat4 model;

void main()
{
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = aTexCoord;
}
