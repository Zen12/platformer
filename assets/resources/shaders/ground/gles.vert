#version 300 es
precision highp float;

layout (location = 0) in vec3 aPos;
layout (location = 4) in vec4 aModelCol0;
layout (location = 5) in vec4 aModelCol1;
layout (location = 6) in vec4 aModelCol2;
layout (location = 7) in vec4 aModelCol3;

out vec4 FragPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightView;
uniform mat4 lightProjection;

// Y-based depth offset for 2.5D sorting (set via uniform, 0 = disabled)
uniform float yDepthFactor;

void main()
{
    mat4 model = mat4(aModelCol0, aModelCol1, aModelCol2, aModelCol3);
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;
    gl_Position.z += worldPos.y * yDepthFactor * gl_Position.w;
    FragPosLightSpace = lightProjection * lightView * worldPos;
}
