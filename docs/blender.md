# Blender MCP Server

This MCP server provides Blender integration for Claude Code, allowing you to control Blender through natural language commands.

## Features

- **Execute Blender Python**: Run arbitrary Python code in Blender with full access to the bpy API
- **Create Objects**: Easily create basic 3D objects (cubes, spheres, cylinders, etc.)
- **Save Files**: Save Blender scenes to .blend files
- **Export GLB**: Export models to GLB format (optimized for game engine)
- **Get Info**: View Blender version and system information
- **List Objects**: List all objects in a scene or .blend file

## Installation

The Blender MCP server is already configured in `.mcp.json`. Just make sure:

1. Blender is installed (detected at: `/opt/homebrew/bin/blender`)
2. The Python virtual environment is set up with fastmcp installed

## Usage Examples

### Through Claude Code

Simply ask Claude to:

```
"Create a cube in Blender at position (2, 3, 5)"
"Execute this Blender Python: import bpy; bpy.ops.mesh.primitive_sphere_add()"
"List all objects in the default Blender scene"
"Save the current scene to models/my_scene.blend"
"Get Blender version information"
```

### Advanced Blender Python

You can execute complex Blender operations:

```python
# Create a scene with multiple objects
import bpy

# Clear existing objects
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete()

# Create a floor
bpy.ops.mesh.primitive_plane_add(size=10, location=(0, 0, 0))
floor = bpy.context.active_object
floor.name = "Floor"

# Create pillars
for x in range(-4, 5, 2):
    for y in range(-4, 5, 2):
        bpy.ops.mesh.primitive_cylinder_add(
            radius=0.3,
            depth=3,
            location=(x, y, 1.5)
        )
        pillar = bpy.context.active_object
        pillar.name = f"Pillar_{x}_{y}"

print(f"Created {len(bpy.data.objects)} objects")
```

### Testing

Run the test script to verify Blender integration:

```bash
cd mcp_tools
../venv/bin/python test_blender.py
```

## Configuration

The Blender path can be customized by setting the `BLENDER_PATH` environment variable in `.mcp.json`:

```json
{
  "mcpServers": {
    "blender": {
      "env": {
        "BLENDER_PATH": "/path/to/your/blender"
      }
    }
  }
}
```

## Technical Details

- Runs Blender in background mode (no GUI) for script execution
- Uses temporary files to pass Python scripts to Blender
- Captures stdout/stderr for debugging
- 60 second timeout for long-running operations
- Filters out common Blender startup messages for cleaner output

## Supported Object Types

When using `create_blender_object()`:

- `cube` - Cube mesh
- `sphere` - UV sphere mesh
- `cylinder` - Cylinder mesh
- `cone` - Cone mesh
- `torus` - Torus mesh
- `plane` - Plane mesh
- `monkey` - Suzanne (monkey head) mesh
- `camera` - Camera object
- `light` - Light object

## Troubleshooting

### Blender not found

If you see "Blender not found" errors, update the `BLENDER_PATH` in `.mcp.json` to point to your Blender installation:

```bash
# Find Blender path
which blender
```

### Import errors

If you see `ModuleNotFoundError`, make sure the fastmcp package is installed:

```bash
./venv/bin/pip install fastmcp
```

## Examples

### Create a Simple Scene

```python
import bpy

# Clear default scene
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete()

# Add sun lamp
bpy.ops.object.light_add(type='SUN', location=(5, 5, 10))

# Add camera
bpy.ops.object.camera_add(location=(7, -7, 5))
camera = bpy.context.active_object
camera.rotation_euler = (1.1, 0, 0.785)

# Add ground plane
bpy.ops.mesh.primitive_plane_add(size=20, location=(0, 0, 0))

# Add monkey head
bpy.ops.mesh.primitive_monkey_add(location=(0, 0, 1))

# Save
bpy.ops.wm.save_as_mainfile(filepath="simple_scene.blend")
print("Scene created and saved!")
```

### Generate Procedural Geometry

```python
import bpy
import math

# Create spiral of cubes
for i in range(20):
    angle = i * 0.5
    radius = i * 0.3
    x = math.cos(angle) * radius
    y = math.sin(angle) * radius
    z = i * 0.2

    bpy.ops.mesh.primitive_cube_add(
        size=0.5,
        location=(x, y, z)
    )
    cube = bpy.context.active_object
    cube.rotation_euler = (angle, angle, angle)
    cube.name = f"Cube_{i}"

print(f"Created {len(bpy.data.objects)} objects in spiral pattern")
```

## Exporting for the Game Engine

### ⚠️ IMPORTANT: Check note.txt Files First

**Before exporting any model, check if a `note.txt` file exists in the model directory.**

These files contain specific export settings required for that model/animation:

```bash
# Check for note.txt
ls assets/resources/models/zombie/original/note.txt

# Read it if it exists
cat assets/resources/models/zombie/original/note.txt
```

**Example note.txt content:**
- Export settings (e.g., `--no-yup` for Z-up orientation)
- Required parameters (scale, compression, etc.)
- Model-specific instructions

**Always follow the settings in note.txt if present!**

### GLB Format (Recommended)

**GLB (GL Binary)** is the recommended format for the game engine. It provides:

- ⚡ **5-50x faster loading** compared to FBX
- 📦 **Smaller file sizes** - efficient binary encoding
- 🎮 **Optimized for real-time** - data ready for GPU upload
- 🌐 **Industry standard** - supported by Unity, Unreal, Godot, Three.js

### Using the MCP Tool (Easiest)

**You can use the `export_to_glb()` MCP tool directly from Claude Code:**

```
# Ask Claude Code:
"Export assets/resources/models/my_model.blend to GLB"

# Claude will use:
export_to_glb(
    input_blend="assets/resources/models/my_model.blend",
    output_glb="assets/resources/models/my_model.glb"
)

# With animations:
export_to_glb(
    input_blend="assets/resources/models/character.blend",
    output_glb="assets/resources/models/character.glb",
    export_animations=True
)

# With compression (for large models):
export_to_glb(
    input_blend="assets/resources/models/large_model.blend",
    output_glb="assets/resources/models/large_model.glb",
    use_compression=True,
    compression_level=6
)

# With custom scale (e.g., scale up by 10x):
export_to_glb(
    input_blend="assets/resources/models/small_model.blend",
    output_glb="assets/resources/models/small_model.glb",
    scale_factor=10.0
)

# For Z-up models (as specified in note.txt):
export_to_glb(
    input_blend="assets/resources/models/zombie/original/Zombie@Z_Idle.FBX",
    output_glb="assets/resources/models/zombie/idle.glb",
    export_animations=True,
    export_yup=False  # Preserve Z-up orientation
)
```

### Using the Export Script

**A reusable export script is available at `mcp_tools/export_to_glb.py`** for command-line use:

```bash
# Basic export
blender --background --python mcp_tools/export_to_glb.py -- \\
    assets/resources/models/my_model.blend \\
    assets/resources/models/my_model.glb

# Export with Draco compression (for large models)
blender --background --python mcp_tools/export_to_glb.py -- \\
    assets/resources/models/large_model.blend \\
    assets/resources/models/large_model.glb \\
    --compress --compression-level 6

# Export with animations
blender --background --python mcp_tools/export_to_glb.py -- \\
    assets/resources/models/character.blend \\
    assets/resources/models/character.glb \\
    --animations

# Export with custom scale (scale up by 10x)
blender --background --python mcp_tools/export_to_glb.py -- \\
    assets/resources/models/small_model.blend \\
    assets/resources/models/small_model.glb \\
    --scale 10.0

# Export Z-up model (preserve orientation, as per note.txt)
blender --background --python mcp_tools/export_to_glb.py -- \\
    assets/resources/models/zombie/original/Zombie@Z_Idle.FBX \\
    assets/resources/models/zombie/idle.glb \\
    --animations --no-yup
```

**As Python module:**

```python
from mcp_tools.export_to_glb import export_to_glb

# Basic export
export_to_glb("model.blend", "model.glb")

# With compression
export_to_glb("model.blend", "model.glb", use_compression=True, compression_level=6)

# With animations
export_to_glb("model.blend", "model.glb", export_animations=True)

# With custom scale
export_to_glb("model.blend", "model.glb", scale_factor=10.0)

# For Z-up models (check note.txt)
export_to_glb("zombie.fbx", "zombie.glb", export_animations=True, export_yup=False)
```

See the script's docstring for complete usage and options.

### Export Options Reference

| Option | CLI Flag | Python Param | Default | Description |
|--------|----------|--------------|---------|-------------|
| Compression | `--compress` | `use_compression=True` | False | Enable Draco mesh compression |
| Compression Level | `--compression-level N` | `compression_level=N` | 6 | Compression level 0-10 |
| Animations | `--animations` | `export_animations=True` | False | Include animations |
| Selection Only | `--selection-only` | `export_selection_only=True` | False | Export selected objects only |
| Apply Modifiers | `--no-apply-modifiers` | `apply_modifiers=False` | True | Apply modifiers before export |
| Scale Factor | `--scale N` | `scale_factor=N` | 1.0 | Scale mesh and bones |
| Y-up Conversion | `--no-yup` | `export_yup=False` | True | Convert to Y-up (False = keep Z-up) |

### note.txt File Format

Place a `note.txt` file in your model directory to document export settings:

```
Model Name Export Settings
===========================

IMPORTANT: Special export requirements for this model.

Correct Export Settings:
------------------------
blender --background --python mcp_tools/export_to_glb.py -- \
    input.fbx \
    output.glb \
    --animations --no-yup --scale 10.0

Or via MCP tool:
export_to_glb("input.fbx", "output.glb", export_animations=True,
              export_yup=False, scale_factor=10.0)

Notes:
------
- Explain why these settings are needed
- List all animations in this set
- Mark which ones are already exported correctly (✓)
```

### Manual GLB Export (Inline)

```python
import bpy

# Export current scene to GLB
bpy.ops.export_scene.gltf(
    filepath="/path/to/model.glb",
    export_format='GLB'  # Binary format (single file)
)
```

### Complete Export Example

```python
import bpy

# Create a simple model
bpy.ops.mesh.primitive_cube_add(location=(0, 0, 0))
cube = bpy.context.active_object
cube.scale = (2, 1, 1)

# Add material
mat = bpy.data.materials.new(name="CubeMaterial")
mat.diffuse_color = (1.0, 0.0, 0.0, 1.0)  # Red
cube.data.materials.append(mat)

# Save as .blend (editable)
bpy.ops.wm.save_as_mainfile(filepath="assets/resources/models/my_model.blend")

# Export as .glb (for game engine)
bpy.ops.export_scene.gltf(
    filepath="assets/resources/models/my_model.glb",
    export_format='GLB'
)

print("✅ Model saved as .blend and exported as .glb")
```

### Export with Compression (Large Models)

For models with high polygon counts (>100k vertices):

```python
bpy.ops.export_scene.gltf(
    filepath="assets/resources/models/large_model.glb",
    export_format='GLB',
    export_draco_mesh_compression_enable=True,  # Enable compression
    export_draco_mesh_compression_level=6        # 0-10, higher = smaller
)
```

### Coordinate System

The game engine uses **Y-forward, Z-up** coordinates, while Blender uses **Y-up** by default.

✅ **The mesh loader automatically converts coordinates**, so you can:
- Model in Blender's Y-up coordinate system (natural)
- Export with default settings
- Engine handles the conversion

### Export Checklist

When exporting models for the game:

- ✅ Use **GLB format** (not FBX)
- ✅ **Apply modifiers** before export
- ✅ Check **scale** - 1 Blender unit = 1 game unit
- ✅ **Center objects** at origin for proper rotation
- ✅ Use **PBR materials** for proper rendering
- ✅ Keep **polygon count reasonable** (<100k vertices for real-time)

### Performance Tips

| Model Complexity | Vertex Count | Export Settings |
|-----------------|--------------|-----------------|
| Simple (props) | <5k | Default GLB |
| Medium (characters) | 5k-50k | Default GLB |
| Complex (environments) | 50k-500k | GLB + Draco compression |
| Very High | >500k | Consider decimation first |

### After Export

1. **Generate metadata** - Asset system needs .meta file
2. **Test loading** - Run debug build to check load times
3. **Check bounds** - Verify model dimensions are correct

Example workflow:
```bash
# 1. Export from Blender (GLB)
# 2. Generate metadata
python3 -m mcp_tools.server.asset_server

# 3. Test loading
./run_debug.sh
# Check console for: "MESH_LOADER: Loaded ... Total: Xms"
```
