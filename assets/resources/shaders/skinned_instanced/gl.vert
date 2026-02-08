#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aBoneWeights;
layout (location = 3) in ivec4 aBoneIndices;

layout (location = 4) in vec4 aModelCol0;
layout (location = 5) in vec4 aModelCol1;
layout (location = 6) in vec4 aModelCol2;
layout (location = 7) in vec4 aModelCol3;
layout (location = 8) in vec4 aInstanceColor;
layout (location = 9) in int aBoneOffset;

out vec2 TexCoord;
out vec4 FragPosLightSpace;
out vec4 InstanceColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightView;
uniform mat4 lightProjection;
uniform sampler2D boneMatrices;

// Texture layout: 4096 pixels wide, 1024 bones per row (each bone = 4 pixels)
const int BONES_PER_ROW = 1024;

mat4 fetchBoneMatrix(int boneIndex) {
    int globalBoneIndex = aBoneOffset + boneIndex;
    int row = globalBoneIndex / BONES_PER_ROW;
    int colOffset = (globalBoneIndex - row * BONES_PER_ROW) * 4;
    return mat4(
        texelFetch(boneMatrices, ivec2(colOffset + 0, row), 0),
        texelFetch(boneMatrices, ivec2(colOffset + 1, row), 0),
        texelFetch(boneMatrices, ivec2(colOffset + 2, row), 0),
        texelFetch(boneMatrices, ivec2(colOffset + 3, row), 0)
    );
}

// Y-based depth offset for 2.5D sorting (set via uniform, 0 = disabled)
uniform float yDepthFactor;

void main()
{
    mat4 model = mat4(aModelCol0, aModelCol1, aModelCol2, aModelCol3);

    mat4 boneTransform = fetchBoneMatrix(aBoneIndices[0]) * aBoneWeights[0];
    boneTransform += fetchBoneMatrix(aBoneIndices[1]) * aBoneWeights[1];
    boneTransform += fetchBoneMatrix(aBoneIndices[2]) * aBoneWeights[2];
    boneTransform += fetchBoneMatrix(aBoneIndices[3]) * aBoneWeights[3];

    vec4 skinnedPos = boneTransform * vec4(aPos, 1.0);
    vec4 worldPos = model * skinnedPos;
    gl_Position = projection * view * worldPos;
    gl_Position.z += worldPos.y * yDepthFactor * gl_Position.w;

    TexCoord = aTexCoord;
    FragPosLightSpace = lightProjection * lightView * worldPos;
    InstanceColor = aInstanceColor;
}
