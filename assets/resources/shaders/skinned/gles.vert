#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aBoneWeights;
layout (location = 3) in ivec4 aBoneIndices;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 boneMatrices[100];  // Support up to 100 bones

// Y-based depth offset for 2.5D sorting (set via uniform, 0 = disabled)
uniform float yDepthFactor;

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
    gl_Position.z += worldPos.y * yDepthFactor * gl_Position.w;

    TexCoord = aTexCoord;
}
