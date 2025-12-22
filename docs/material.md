# Material Creation Guide

## Overview

Materials (.mat files) link together shaders and optionally reference textures and fonts. This guide explains how to create and update materials using the Python script and MCP tools.

## Material Structure

A material file contains:
- **name**: Material identifier
- **shader_opengl**: OpenGL shader references (vertex + fragment GUIDs)
- **shader_gles**: GLES shader references (vertex + fragment GUIDs)
- **blend_mode**: Integer (0=none, 1=alpha blending, etc.)
- **is_culling**: Boolean (enable/disable face culling)
- **image**: Optional texture GUID
- **font**: Optional font GUID

## Python API

### create_material()

```python
from create import create_material

mat_path, meta_path, mat_guid = create_material(
    name="my_material",
    opengl_vertex_guid="vertex-shader-guid",
    opengl_fragment_guid="fragment-shader-guid",
    gles_vertex_guid="vertex-shader-guid",
    gles_fragment_guid="fragment-shader-guid",
    image_guid="optional-texture-guid",
    font_guid="optional-font-guid",
    blend_mode=1,
    is_culling=False
)
```

**Parameters:**
- `name` (str): Material name (used for filename)
- `opengl_vertex_guid` (str): GUID of OpenGL vertex shader
- `opengl_fragment_guid` (str): GUID of OpenGL fragment shader
- `gles_vertex_guid` (str): GUID of GLES vertex shader
- `gles_fragment_guid` (str): GUID of GLES fragment shader
- `image_guid` (str, optional): Texture image GUID
- `font_guid` (str, optional): Font GUID
- `blend_mode` (int): Blend mode (default: 0)
- `is_culling` (bool): Enable face culling (default: False)
- `output_path` (str, optional): Custom output path

**Returns:**
- `mat_path`: Path to created .mat file
- `meta_path`: Path to created .meta file
- `mat_guid`: Material GUID (preserved if updating existing material)

## MCP Tool Usage

The `create_material_file` MCP tool is available for easy material creation:

```python
# Example usage via MCP
create_material_file(
    name="sprite_material",
    opengl_vertex_guid="79445870-71ce-4f77-aa7f-e8003933421e",
    opengl_fragment_guid="81c8f433-60ca-4485-9664-3ec62b0bebe0",
    gles_vertex_guid="22132622-1d08-4b2b-982c-8ee823b7095b",
    gles_fragment_guid="7e7a515d-5735-4faa-a5cd-0438d7a8d9e9",
    blend_mode=1,
    is_culling=False
)
```

## Finding Shader GUIDs

To get shader GUIDs, check the .meta files:

```bash
# OpenGL shaders
cat assets/resources/shaders/opengl/shader_name.vert.meta
cat assets/resources/shaders/opengl/shader_name.frag.meta

# GLES shaders
cat assets/resources/shaders/gles/shader_name.vert.meta
cat assets/resources/shaders/gles/shader_name.frag.meta
```

## Updating Existing Materials

When you call `create_material()` with a name that already exists:
- The GUID is preserved from the existing .meta file
- The material file is overwritten with new shader references
- All references to the material in scenes/prefabs remain valid

## Example: Complete Workflow

```python
# 1. Create or get shader GUIDs
# (Use create_shader_files MCP tool or check existing shader .meta files)

opengl_vert = "79445870-71ce-4f77-aa7f-e8003933421e"
opengl_frag = "81c8f433-60ca-4485-9664-3ec62b0bebe0"
gles_vert = "22132622-1d08-4b2b-982c-8ee823b7095b"
gles_frag = "7e7a515d-5735-4faa-a5cd-0438d7a8d9e9"

# 2. Create material
from create import create_material

mat_path, meta_path, guid = create_material(
    name="custom_sprite",
    opengl_vertex_guid=opengl_vert,
    opengl_fragment_guid=opengl_frag,
    gles_vertex_guid=gles_vert,
    gles_fragment_guid=gles_frag,
    blend_mode=1,
    is_culling=False
)

print(f"Created material with GUID: {guid}")
print(f"Material file: {mat_path}")
```

## Blend Modes

Common blend mode values:
- `0`: No blending (opaque)
- `1`: Alpha blending (transparency)
- Other values may be engine-specific

## Directory Structure

**IMPORTANT RULE:** All assets MUST be created under `assets/resources/` directory, except `project.yaml` which goes in `assets/` root.

Correct structure:
```
assets/
├── project.yaml                          # Project configuration (root level only)
└── resources/                            # All other assets go here
    ├── shaders/                          # Shader files
    │   └── shader_name/
    │       ├── gl.vert                   # OpenGL vertex shader
    │       ├── gl.vert.meta
    │       ├── gl_.frag                  # OpenGL fragment shader
    │       ├── gl_.frag.meta
    │       ├── gles.vert                 # GLES vertex shader
    │       ├── gles.vert.meta
    │       ├── gles_.frag                # GLES fragment shader
    │       └── gles_.frag.meta
    ├── materials/                        # Material files
    │   ├── material_name.mat
    │   └── material_name.mat.meta
    ├── scenes/                           # Scene files
    ├── prefabs/                          # Prefab files
    ├── images/                           # Texture images
    ├── models/                           # 3D models
    ├── fonts/                            # Font files
    └── fsm/                              # FSM files
```

## Related Tools

- **create_shader_files**: Create new shaders with OpenGL and GLES versions
- **generate_asset_metadata**: Generate .meta files for assets without them
- **check_asset_references**: Check what references a material before deleting
