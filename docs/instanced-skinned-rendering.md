# Instanced Skinned Mesh Rendering

## Overview

GPU-instanced rendering for 1000+ animated skinned meshes. Reduces draw calls from N to 1 per batch using Texture Buffer Objects (TBO) for bone matrix storage.

**Performance**: 1000 instances = 1 draw call (was 1000 draw calls)

## Architecture

```
Before: CPU uploads 100 bones per instance -> 1000 glDrawElements calls
After:  CPU uploads all bones to TBO -> 1 glDrawElementsInstanced call
```

## Files Created

### Instancing Infrastructure

| File | Purpose |
|------|---------|
| `src/engine/renderer/instancing/bone_matrix_buffer.hpp/.cpp` | TBO wrapper for bone matrices |
| `src/engine/renderer/instancing/instance_buffer.hpp/.cpp` | Per-instance VBO (model matrix + bone offset) |
| `src/engine/renderer/instancing/skinned_instance_batch.hpp/.cpp` | Coordinates bone + instance buffers |

### Shaders

| File | GUID |
|------|------|
| `assets/resources/shaders/skinned_instanced/gl.vert` | `48d7d1db-ad04-40e4-9148-ea00618e618a` |
| `assets/resources/shaders/skinned_instanced/gl_.frag` | `0e167558-99ae-4ca1-8f60-bc068107ff65` |

## Files Modified

| File | Changes |
|------|---------|
| `src/engine/renderer/mesh/mesh.hpp/.cpp` | Added `ConfigureInstanceAttributes()`, `HasInstanceAttributes()` |
| `src/engine/renderer/render_repository.hpp` | Added `IsSkinned` flag to `RenderData` and `RenderId` |
| `src/engine/renderer/material/material.hpp` | Added `BindTextures()`, `ApplyRenderState()` |
| `src/engine/renderer/controller/opengl_render_controller.hpp/.cpp` | Added instanced rendering path |
| `src/engine/esc/skinned_mesh_renderer/skinned_mesh_render_system.hpp` | Marks skinned meshes with `IsSkinned=true` |

## Technical Details

### Bone Matrix Storage (TBO)

```cpp
// Layout: [Instance0 Bones][Instance1 Bones]...
// Each bone = 4 texels (mat4 columns as vec4)
// Format: GL_RGBA32F

class BoneMatrixBuffer {
    GLuint _tbo;        // Buffer object
    GLuint _textureId;  // Texture for samplerBuffer

    void Resize(size_t instanceCount);
    void Upload(const std::vector<glm::mat4>& allBones);
    void Bind(GLuint textureUnit);
};
```

### Per-Instance Data (VBO)

```cpp
struct InstanceVertexData {
    glm::mat4 ModelMatrix;  // locations 4-7 (4 vec4s)
    int32_t BoneOffset;     // location 8
    int32_t Padding[3];     // alignment
};

// Uses glVertexAttribDivisor(1) for per-instance attributes
```

### Vertex Shader

```glsl
#version 330 core

// Per-vertex
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in ivec4 aBoneIds;
layout (location = 4) in vec4 aWeights;

// Per-instance (divisor = 1)
layout (location = 5) in vec4 aModelCol0;
layout (location = 6) in vec4 aModelCol1;
layout (location = 7) in vec4 aModelCol2;
layout (location = 8) in vec4 aModelCol3;
layout (location = 9) in int aBoneOffset;

uniform samplerBuffer boneMatrices;  // TBO

mat4 fetchBoneMatrix(int boneIndex) {
    int offset = (aBoneOffset + boneIndex) * 4;
    return mat4(
        texelFetch(boneMatrices, offset),
        texelFetch(boneMatrices, offset + 1),
        texelFetch(boneMatrices, offset + 2),
        texelFetch(boneMatrices, offset + 3)
    );
}
```

### Render Path

```cpp
// In OpenGLRenderController::Render()
if (renderId.IsSkinned && instances.size() > 1) {
    RenderSkinnedInstanced(renderId, instances);  // Single draw call
} else {
    // Original per-instance rendering
}
```

## Memory Usage

| Component | Per Instance | 1000 Instances |
|-----------|--------------|----------------|
| Bone TBO (100 bones) | 6,400 bytes | 6.4 MB |
| Instance VBO | 68 bytes | 68 KB |
| **Total** | ~6.5 KB | ~6.5 MB |

## OpenGL Requirements

| Feature | Required | macOS 4.1 |
|---------|----------|-----------|
| Texture Buffer Object | GL 3.1 | Yes |
| glDrawElementsInstanced | GL 3.1 | Yes |
| glVertexAttribDivisor | GL 3.3 | Yes |
| samplerBuffer | GL 3.1 | Yes |

## Usage

### Scene Configuration

```yaml
# test.scene
entities:
  - tag: "zombie_spawner"
    components:
      - type: spawner
        prefab_guid: ee9e7ec4-779d-4d44-a4f6-99f048412b61
        spawn_count: 2000  # Instanced automatically
        spawn_on_navmesh: true

navmesh:
  max_agents: 2100  # Must be >= spawn_count
```

### How It Works

1. `SkinnedMeshRenderSystem` adds render data with `IsSkinned=true`
2. `RenderRepository` batches by Material+Mesh+Camera
3. `OpenGLRenderController` detects skinned batches with >1 instance
4. `SkinnedInstanceBatch` collects all bone matrices and model matrices
5. Single `glDrawElementsInstanced()` call renders all instances

## Debugging

### Verify Instancing is Active

Add logging in `OpenGLRenderController::RenderSkinnedInstanced()`:
```cpp
std::cout << "Instanced render: " << batch->GetInstanceCount() << " instances\n";
```

### Common Issues

| Issue | Cause | Fix |
|-------|-------|-----|
| Backfaces rendering | Missing `ApplyRenderState()` | Call `mat->ApplyRenderState()` before draw |
| No instances rendered | Shader not found | Check shader GUIDs match |
| Wrong transforms | Bone offset calculation | Verify `aBoneOffset` in shader |

## Future Improvements

1. **Frustum culling** - Skip instances outside view (currently disabled)
2. **LOD support** - Different meshes at distance
3. **Animation LOD** - Reduce bone updates for distant instances
4. **Indirect rendering** - GPU-driven culling with `glDrawElementsIndirect`
