#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

layout (location = 4) in vec4 aModelCol0;
layout (location = 5) in vec4 aModelCol1;
layout (location = 6) in vec4 aModelCol2;
layout (location = 7) in vec4 aModelCol3;
layout (location = 8) in vec4 aInstanceColor;

out vec2 TexCoord;
out vec4 FragPosLightSpace;
out vec4 InstanceColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightView;
uniform mat4 lightProjection;

void main()
{
    mat4 model = mat4(aModelCol0, aModelCol1, aModelCol2, aModelCol3);
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;
    TexCoord = aTexCoord;
    FragPosLightSpace = lightProjection * lightView * worldPos;
    InstanceColor = aInstanceColor;
}
