# Instancing System

This document describes the GPU instancing system used for efficient rendering of multiple objects with the same mesh and material.

## Overview

The instancing system allows rendering many instances of the same mesh in a single draw call, significantly reducing CPU overhead. It supports both static meshes and skinned (animated) meshes.

## Architecture

### Key Components

```
InstanceBatch          - Manages instance data (model matrices, bone transforms)
├── InstanceBuffer     - GPU buffer for per-instance vertex data (locations 4-8)
└── BoneMatrixBuffer   - GPU texture for bone matrices (optional, for skinned meshes)

Material               - Provides shader and render state
OpenGLRenderController - Orchestrates rendering with RenderInstanced()
```

### Data Flow

```
1. Game systems submit RenderData to RenderRepository
2. RenderRepository groups instances by (Material, Mesh, Camera, IsSkinned)
3. OpenGLRenderController::Render() iterates groups
4. RenderInstanced() batches instances and issues single draw call
```

## Shader Attribute Locations

All instanced shaders must use these attribute locations:

| Location | Attribute | Type | Description |
|----------|-----------|------|-------------|
| 0 | aPos | vec3 | Vertex position |
| 1 | aTexCoord | vec2 | Texture coordinates |
| 2 | aBoneWeights | vec4 | Bone weights (skinned only) |
| 3 | aBoneIndices | ivec4 | Bone indices (skinned only) |
| 4-7 | aModelCol0-3 | vec4 | Model matrix (4 columns) |
| 8 | aBoneOffset | int | Bone texture offset (skinned only) |

### Static Mesh Shader Example

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

layout (location = 4) in vec4 aModelCol0;
layout (location = 5) in vec4 aModelCol1;
layout (location = 6) in vec4 aModelCol2;
layout (location = 7) in vec4 aModelCol3;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    mat4 model = mat4(aModelCol0, aModelCol1, aModelCol2, aModelCol3);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
```

### Skinned Mesh Shader Example

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aBoneWeights;
layout (location = 3) in ivec4 aBoneIndices;

layout (location = 4) in vec4 aModelCol0;
layout (location = 5) in vec4 aModelCol1;
layout (location = 6) in vec4 aModelCol2;
layout (location = 7) in vec4 aModelCol3;
layout (location = 8) in int aBoneOffset;

uniform mat4 projection;
uniform mat4 view;
uniform sampler2D boneMatrices;

// Bone texture: 4096 pixels wide, 1024 bones per row (4 pixels per bone)
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

void main()
{
    mat4 model = mat4(aModelCol0, aModelCol1, aModelCol2, aModelCol3);

    mat4 boneTransform = fetchBoneMatrix(aBoneIndices[0]) * aBoneWeights[0];
    boneTransform += fetchBoneMatrix(aBoneIndices[1]) * aBoneWeights[1];
    boneTransform += fetchBoneMatrix(aBoneIndices[2]) * aBoneWeights[2];
    boneTransform += fetchBoneMatrix(aBoneIndices[3]) * aBoneWeights[3];

    vec4 skinnedPos = boneTransform * vec4(aPos, 1.0);
    gl_Position = projection * view * model * skinnedPos;
}
```

## InstanceBatch Class

### Header (instance_batch.hpp)

```cpp
class InstanceBatch {
public:
    void Clear();                                    // Reset for new frame
    void AddInstance(const glm::mat4& modelMatrix);  // Static instance
    void AddInstance(const glm::mat4& modelMatrix,   // Skinned instance
                     const std::vector<glm::mat4>& boneTransforms);
    void Finalize();                                 // Upload to GPU

    size_t GetInstanceCount() const;
    bool HasBones() const;                           // True if any skinned instances

    void BindBoneTexture(GLuint textureUnit) const;  // Bind bone data
    void SetupInstanceAttributes() const;            // Setup VAO attributes
};
```

### Usage Pattern

```cpp
auto& batch = _batches[batchKey];
batch->Clear();

for (const auto& inst : instances) {
    if (isSkinned && inst.BoneTransforms.has_value()) {
        batch->AddInstance(inst.ModelMatrix, inst.BoneTransforms.value());
    } else {
        batch->AddInstance(inst.ModelMatrix);
    }
}

batch->Finalize();

meshPtr->Bind();
batch->SetupInstanceAttributes();

// ... set uniforms ...

if (batch->HasBones()) {
    batch->BindBoneTexture(BONE_TEXTURE_UNIT);
    mat->SetInt("boneMatrices", BONE_TEXTURE_UNIT);
}

glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
```

## Material Integration

Materials use their shader via the Material API:

```cpp
mat->UseShader();
mat->SetMat4("view", cameraView);
mat->SetMat4("projection", cameraProjection);
mat->ApplyRenderState();  // Culling, depth test, blend mode

if (mat->HasTextures()) {
    mat->BindTextures();
    mat->SetInt("texture1", 0);
}
```

### Required Uniforms

| Uniform | Type | Description |
|---------|------|-------------|
| view | mat4 | Camera view matrix |
| projection | mat4 | Camera projection matrix |
| texture1 | sampler2D | Diffuse texture (if material has textures) |
| boneMatrices | sampler2D | Bone matrix texture (if skinned) |

## Adding New Instanced Materials

1. **Create shader** with attribute locations 4-7 for model matrix
2. **Create material file** referencing the shader
3. **Ensure shader has** `view` and `projection` uniforms
4. **Add `texture1` uniform** if material uses textures

Example material file:
```yaml
name: my_material
is_culling: true
is_depth_test: true
blend_mode: 0
shader_opengl:
  vertex: <vertex-shader-guid>
  fragment: <fragment-shader-guid>
shader_gles:
  vertex: <gles-vertex-shader-guid>
  fragment: <gles-fragment-shader-guid>
image: <texture-guid>  # optional
```

## Performance Considerations

1. **Batching**: Instances are grouped by (Material + Mesh + Camera). More unique combinations = more draw calls.

2. **Bone Buffer**: Created lazily only when skinned instances are added. Static-only batches have no bone overhead.

3. **Buffer Reuse**: Batches are cached by key and reused each frame. Only data is updated, not buffer allocation.

4. **Texture Units**:
   - Unit 0: Diffuse texture
   - Unit 1: Bone matrix texture (skinned only)

## File Locations

```
src/engine/renderer/instancing/
├── instance_batch.hpp       # Unified batch class
├── instance_batch.cpp
├── instance_buffer.hpp      # Per-instance vertex data buffer
├── instance_buffer.cpp
├── bone_matrix_buffer.hpp   # Bone matrices as texture
└── bone_matrix_buffer.cpp

src/engine/renderer/controller/
└── opengl_render_controller.cpp  # RenderInstanced() method

assets/resources/shaders/
├── textured/                # Static textured meshes
├── solid_color/             # Static solid color meshes
└── skinned_instanced/       # Skinned animated meshes
```
