#version 300 es

precision mediump float;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec3 localPosition;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;


void main()
{
    vec4 pos = projection * view * vec4(aPos, 1.0);
    localPosition = aPos.xyz;
    gl_Position = pos;
}