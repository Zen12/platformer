#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aBoneWeights;
layout (location = 3) in ivec4 aBoneIndices;

out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 boneMatrices[100];  // Support up to 100 bones

void main()
{
    // Calculate skinned position
    mat4 boneTransform = boneMatrices[aBoneIndices[0]] * aBoneWeights[0];
    boneTransform += boneMatrices[aBoneIndices[1]] * aBoneWeights[1];
    boneTransform += boneMatrices[aBoneIndices[2]] * aBoneWeights[2];
    boneTransform += boneMatrices[aBoneIndices[3]] * aBoneWeights[3];

    vec4 skinnedPos = boneTransform * vec4(aPos, 1.0);
    vec4 worldPos = model * skinnedPos;
    gl_Position = projection * view * worldPos;

    WorldPos = worldPos.xyz;
}
