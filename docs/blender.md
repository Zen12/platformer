# Blender MCP Server

This MCP server provides Blender integration for Claude Code, allowing you to control Blender through natural language commands.

## Features

- **Execute Blender Python**: Run arbitrary Python code in Blender with full access to the bpy API
- **Create Objects**: Easily create basic 3D objects (cubes, spheres, cylinders, etc.)
- **Save Files**: Save Blender scenes to .blend files
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
