"""
MCP Server for Blender Integration

This server exposes Blender Python API tools to Claude Code.
It allows executing Blender scripts and commands through the bpy API.
"""

import os
import sys
import subprocess
import tempfile
import logging

# Setup logging to stderr (NEVER use stdout with stdio transport)
logging.basicConfig(
    stream=sys.stderr,
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger("blender-server")

from fastmcp import FastMCP

# Initialize MCP server
mcp = FastMCP("blender")

# Blender executable path - configured via environment variable
BLENDER_PATH = os.getenv('BLENDER_PATH', '/opt/homebrew/bin/blender')


def execute_blender_script(script_content: str, background: bool = True) -> tuple[str, str, int]:
    """
    Execute a Python script in Blender.

    Args:
        script_content: Python script to execute in Blender
        background: Run Blender in background mode (no GUI)

    Returns:
        tuple: (stdout, stderr, return_code)
    """
    # Create temporary script file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.py', delete=False) as f:
        f.write(script_content)
        script_path = f.name

    try:
        # Build Blender command
        cmd = [BLENDER_PATH]
        if background:
            cmd.extend(['--background'])
        cmd.extend(['--python', script_path])

        # Execute Blender
        logger.info(f"Executing Blender script: {' '.join(cmd)}")
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=60  # 60 second timeout
        )

        return result.stdout, result.stderr, result.returncode

    finally:
        # Cleanup temp file
        try:
            os.unlink(script_path)
        except Exception as e:
            logger.warning(f"Failed to delete temp file {script_path}: {e}")


@mcp.tool()
def execute_blender_python(code: str) -> str:
    """
    Execute Python code in Blender.

    Runs the provided Python code in a Blender instance with access to the bpy module.
    The code is executed in background mode (no GUI).

    Args:
        code: Python code to execute (can use bpy, bmesh, mathutils, etc.)

    Returns:
        str: Output from the script execution

    Examples:
        - Get Blender version: "import bpy; print(bpy.app.version_string)"
        - Create a cube: "import bpy; bpy.ops.mesh.primitive_cube_add()"
        - List all objects: "import bpy; print([obj.name for obj in bpy.data.objects])"
    """
    logger.info(f"Executing Blender Python code")

    try:
        stdout, stderr, returncode = execute_blender_script(code, background=True)

        # Build result message
        result = ""

        if returncode == 0:
            result += "✓ Script executed successfully\n\n"
        else:
            result += f"⚠️  Script exited with code {returncode}\n\n"

        # Filter out Blender startup messages from stdout
        # Only show lines that look like actual output (not Blender's info messages)
        filtered_stdout = []
        for line in stdout.split('\n'):
            # Skip common Blender startup messages
            if any(skip in line for skip in [
                'Blender',
                'Read prefs:',
                'found bundled python:',
                'Read blend:',
                'Saved session recovery',
                'Blender quit'
            ]):
                continue
            if line.strip():
                filtered_stdout.append(line)

        if filtered_stdout:
            result += "Output:\n"
            result += '\n'.join(filtered_stdout)
            result += "\n"

        # Show stderr if there are errors/warnings
        if stderr:
            # Filter out common harmless warnings
            filtered_stderr = []
            for line in stderr.split('\n'):
                if any(skip in line for skip in [
                    'color management',
                    'ALSA',
                    'OSL',
                    'libpng warning'
                ]):
                    continue
                if line.strip():
                    filtered_stderr.append(line)

            if filtered_stderr:
                result += "\nWarnings/Errors:\n"
                result += '\n'.join(filtered_stderr)
                result += "\n"

        logger.info(f"Blender execution completed with code {returncode}")
        return result.strip() if result.strip() else "Script executed (no output)"

    except subprocess.TimeoutExpired:
        error_msg = "Error: Script execution timed out (60 second limit)"
        logger.error(error_msg)
        return error_msg
    except FileNotFoundError:
        error_msg = f"Error: Blender not found at {BLENDER_PATH}\nSet BLENDER_PATH environment variable to the correct path."
        logger.error(error_msg)
        return error_msg
    except Exception as e:
        error_msg = f"Error executing Blender script: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return error_msg


@mcp.tool()
def create_blender_object(object_type: str, location: list = None, name: str = None) -> str:
    """
    Create a basic object in Blender.

    Args:
        object_type: Type of object to create (cube, sphere, cylinder, cone, torus, plane, monkey, camera, light)
        location: [x, y, z] coordinates (default: [0, 0, 0])
        name: Optional name for the object

    Returns:
        str: Result of object creation
    """
    if location is None:
        location = [0, 0, 0]

    logger.info(f"Creating Blender {object_type} at {location}")

    # Map object types to Blender operators
    object_ops = {
        'cube': 'mesh.primitive_cube_add',
        'sphere': 'mesh.primitive_uv_sphere_add',
        'cylinder': 'mesh.primitive_cylinder_add',
        'cone': 'mesh.primitive_cone_add',
        'torus': 'mesh.primitive_torus_add',
        'plane': 'mesh.primitive_plane_add',
        'monkey': 'mesh.primitive_monkey_add',
        'camera': 'object.camera_add',
        'light': 'object.light_add'
    }

    if object_type.lower() not in object_ops:
        return f"Error: Unknown object type '{object_type}'. Supported types: {', '.join(object_ops.keys())}"

    # Build script
    script = f"""import bpy

# Clear existing mesh objects (optional, comment out if you don't want this)
# bpy.ops.object.select_all(action='SELECT')
# bpy.ops.object.delete()

# Create object
bpy.ops.{object_ops[object_type.lower()]}(location=({location[0]}, {location[1]}, {location[2]}))

# Rename if name provided
"""

    if name:
        script += f"""
active_obj = bpy.context.active_object
if active_obj:
    active_obj.name = "{name}"
    print(f"Created {{active_obj.type}} object: {{active_obj.name}} at {{active_obj.location}}")
"""
    else:
        script += """
active_obj = bpy.context.active_object
if active_obj:
    print(f"Created {active_obj.type} object: {active_obj.name} at {active_obj.location}")
"""

    return execute_blender_python(script)


@mcp.tool()
def save_blender_file(filepath: str, use_compress: bool = False) -> str:
    """
    Save the current Blender scene to a .blend file.

    Args:
        filepath: Path where to save the .blend file
        use_compress: Whether to compress the file (default: False)

    Returns:
        str: Result of save operation
    """
    logger.info(f"Saving Blender file to {filepath}")

    # Ensure filepath is absolute
    filepath = os.path.abspath(filepath)

    # Ensure directory exists
    os.makedirs(os.path.dirname(filepath), exist_ok=True)

    script = f"""import bpy

# Save file
bpy.ops.wm.save_as_mainfile(filepath="{filepath}", compress={use_compress})
print(f"Saved Blender file to: {filepath}")
"""

    return execute_blender_python(script)


@mcp.tool()
def get_blender_info() -> str:
    """
    Get information about the Blender installation.

    Returns:
        str: Blender version and system information
    """
    logger.info("Getting Blender info")

    script = """import bpy
import sys

print(f"Blender Version: {bpy.app.version_string}")
print(f"Python Version: {sys.version}")
print(f"Blender Binary Path: {bpy.app.binary_path}")
"""

    return execute_blender_python(script)


@mcp.tool()
def list_blender_objects(blend_file: str = None) -> str:
    """
    List all objects in a Blender file.

    Args:
        blend_file: Optional path to .blend file (if not provided, uses default empty scene)

    Returns:
        str: List of objects with their types and locations
    """
    logger.info(f"Listing objects in Blender file: {blend_file or 'default scene'}")

    script = """import bpy

print("Objects in scene:")
for obj in bpy.data.objects:
    print(f"  - {obj.name} ({obj.type}) at {obj.location}")

print(f"\\nTotal objects: {len(bpy.data.objects)}")
"""

    if blend_file:
        # If a blend file is provided, load it first
        blend_file = os.path.abspath(blend_file)
        if not os.path.exists(blend_file):
            return f"Error: Blend file not found: {blend_file}"

        script = f"""import bpy

# Load blend file
bpy.ops.wm.open_mainfile(filepath="{blend_file}")

{script}
"""

    return execute_blender_python(script)


@mcp.tool()
def export_to_glb(
    input_blend: str,
    output_glb: str,
    use_compression: bool = False,
    compression_level: int = 6,
    export_animations: bool = False,
    scale_factor: float = 1.0,
    export_yup: bool = True
) -> str:
    """
    Export Blender file to GLB format.

    Exports a .blend file to GLB (GL Binary) format optimized for the game engine.
    GLB provides 5-50x faster loading compared to FBX with smaller file sizes.

    IMPORTANT: Check for note.txt files in the model directory before exporting.
    These files contain specific export settings for that model/animation.

    Args:
        input_blend: Path to Blender file (.blend) or FBX file (.fbx)
        output_glb: Path to save GLB file
        use_compression: Enable Draco mesh compression for large models (default: False)
        compression_level: Compression level 0-10, higher = smaller file (default: 6)
        export_animations: Include animations in export (default: False)
        scale_factor: Scale factor for mesh and bones (default: 1.0)
        export_yup: Convert to Y-up orientation (default: True, set False for Z-up models)

    Returns:
        str: Result of export operation with file size

    Example:
        export_to_glb(
            "assets/resources/models/my_model.blend",
            "assets/resources/models/my_model.glb"
        )

        export_to_glb(
            "assets/resources/models/character.blend",
            "assets/resources/models/character.glb",
            export_animations=True
        )

        export_to_glb(
            "assets/resources/models/small_model.blend",
            "assets/resources/models/small_model.glb",
            scale_factor=10.0
        )

        # For Z-up models (check note.txt if available)
        export_to_glb(
            "assets/resources/models/zombie/original/Zombie@Z_Idle.FBX",
            "assets/resources/models/zombie/idle.glb",
            export_animations=True,
            export_yup=False
        )

    Post-processing:
        After export, run: python3 mcp_tools/import.py to generate .meta file
    """
    logger.info(f"Exporting to GLB: {input_blend} -> {output_glb}")

    # Make paths absolute
    input_blend = os.path.abspath(input_blend)
    output_glb = os.path.abspath(output_glb)

    # Verify input exists
    if not os.path.exists(input_blend):
        return f"Error: Input .blend file not found: {input_blend}"

    # Ensure output directory exists
    os.makedirs(os.path.dirname(output_glb), exist_ok=True)

    # Build export script
    file_ext = os.path.splitext(input_blend)[1].lower()

    script = f"""import bpy
import os

# Load file (detect format)
print("Loading: {input_blend}")
file_ext = "{file_ext}"

if file_ext == '.fbx':
    # Import FBX file
    bpy.ops.import_scene.fbx(filepath="{input_blend}")
    print(f"Imported FBX: {input_blend}")
elif file_ext == '.blend':
    # Open blend file
    bpy.ops.wm.open_mainfile(filepath="{input_blend}")
    print(f"Opened blend file: {input_blend}")
else:
    print(f"Error: Unsupported file format: {{file_ext}}")
    print("Supported formats: .blend, .fbx")
    import sys
    sys.exit(1)

print(f"Loaded {{len(bpy.data.objects)}} objects")

# List objects
for obj in bpy.data.objects:
    print(f"  - {{obj.name}} ({{obj.type}})")

# Apply scale factor if not 1.0
scale_factor = {scale_factor}
if scale_factor != 1.0:
    print(f"Applying scale factor: {{scale_factor}}")
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.transform.resize(value=(scale_factor, scale_factor, scale_factor))

    # Apply scale to bake it into the mesh and armature
    for obj in bpy.data.objects:
        if obj.type in ['MESH', 'ARMATURE']:
            bpy.context.view_layer.objects.active = obj
            bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
            print(f"  Applied scale to: {{obj.name}}")

    print("Scale applied and baked into objects")

# Fix Y-forward to Z-forward for models that use Y as forward axis
# Rotate -90 degrees around Z axis to convert Y-forward to Z-forward
export_yup = {str(export_yup)}
if not export_yup:  # Only for Z-up models (like zombie)
    print("Converting Y-forward to Z-forward (rotating -90° around Z axis)")
    bpy.ops.object.select_all(action='SELECT')
    import math
    bpy.ops.transform.rotate(value=math.radians(-90), orient_axis='Z', orient_type='GLOBAL')

    # Apply rotation to bake it
    for obj in bpy.data.objects:
        if obj.type in ['MESH', 'ARMATURE']:
            bpy.context.view_layer.objects.active = obj
            bpy.ops.object.transform_apply(location=False, rotation=True, scale=False)
            print(f"  Applied rotation to: {{obj.name}}")

    print("Y-forward to Z-forward conversion applied")

# Build export options
export_options = {{
    'filepath': "{output_glb}",
    'export_format': 'GLB',
    'use_selection': False,
    'export_apply': True,
}}

# Animation export
export_animations = {str(export_animations)}
if export_animations:
    export_options['export_animations'] = True
    export_options['export_frame_range'] = True
    export_options['export_force_sampling'] = True
    print("Animation export enabled")
    if bpy.data.actions:
        print(f"Found {{len(bpy.data.actions)}} animation(s):")
        for action in bpy.data.actions:
            print(f"  - {{action.name}}, frames: {{int(action.frame_range[0])}} - {{int(action.frame_range[1])}}")

# Compression
use_compression = {str(use_compression)}
if use_compression:
    export_options['export_draco_mesh_compression_enable'] = True
    export_options['export_draco_mesh_compression_level'] = {compression_level}
    print(f"Draco compression enabled (level {compression_level})")

# Coordinate system conversion
export_yup = {str(export_yup)}
export_options['export_yup'] = export_yup
if not export_yup:
    print("Preserving Z-up orientation (export_yup=False)")
else:
    print("Converting to Y-up orientation (export_yup=True)")

# Export to GLB
print("=== EXPORTING TO GLB ===")
bpy.ops.export_scene.gltf(**export_options)
print("=== EXPORT COMPLETE ===")

# Report file size
if os.path.exists("{output_glb}"):
    size_kb = os.path.getsize("{output_glb}") / 1024
    print(f"✅ Successfully exported to: {output_glb}")
    print(f"   File size: {{size_kb:.1f}} KB")
else:
    print("❌ ERROR: Export failed")
"""

    try:
        stdout, stderr, returncode = execute_blender_script(script, background=True)

        # Build result
        result = ""
        if returncode == 0:
            result += "✓ GLB export successful\n\n"
        else:
            result += f"⚠️  Export failed with code {returncode}\n\n"

        # Filter output
        filtered_lines = []
        for line in stdout.split('\n'):
            if any(skip in line for skip in [
                'Blender',
                'Read prefs:',
                'found bundled python:',
                'Read blend:',
                'Saved session',
                'Blender quit',
                'color management',
                'libpng warning'
            ]):
                continue
            if line.strip():
                filtered_lines.append(line)

        if filtered_lines:
            result += '\n'.join(filtered_lines)
        else:
            result += "No output"

        if stderr and returncode != 0:
            # Filter stderr
            filtered_stderr = []
            for line in stderr.split('\n'):
                if any(skip in line for skip in ['color management', 'ALSA', 'OSL', 'libpng warning']):
                    continue
                if line.strip():
                    filtered_stderr.append(line)
            if filtered_stderr:
                result += "\n\nErrors:\n" + '\n'.join(filtered_stderr)

        logger.info(f"Export completed with code {returncode}")
        return result

    except Exception as e:
        error_msg = f"Error exporting to GLB: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return error_msg


@mcp.tool()
def export_to_glb_baked(
    input_blend: str,
    output_glb: str,
    export_yup: bool = True
) -> str:
    """
    Export Blender file to GLB with transforms baked into mesh vertices.

    This tool bakes object transforms (scale, rotation, location) directly into
    mesh vertex data before export. Use this for static meshes that have non-uniform
    scale or transforms that need to be preserved in the exported mesh data.

    Unlike export_to_glb which stores transforms in the node, this tool:
    - Multiplies all vertex positions by the object's world matrix
    - Resets object transform to identity (scale=1, rotation=0, location=0)
    - Exports the mesh with transforms already applied to vertices

    Args:
        input_blend: Path to Blender file (.blend)
        output_glb: Path to save GLB file
        export_yup: Convert to Y-up orientation (default: True)

    Returns:
        str: Result of export operation with mesh bounds

    Example:
        export_to_glb_baked(
            "assets/resources/models/ground/original/ground2.blend",
            "assets/resources/models/ground/ground2.glb"
        )

    Use cases:
        - Ground/terrain meshes with large scale values
        - Static props with non-uniform scale
        - Any mesh where you want transforms baked into vertex data
    """
    logger.info(f"Exporting to GLB (baked): {input_blend} -> {output_glb}")

    # Make paths absolute
    input_blend = os.path.abspath(input_blend)
    output_glb = os.path.abspath(output_glb)

    # Verify input exists
    if not os.path.exists(input_blend):
        return f"Error: Input .blend file not found: {input_blend}"

    # Ensure output directory exists
    os.makedirs(os.path.dirname(output_glb), exist_ok=True)

    script = f"""import bpy
from mathutils import Vector

# Clear and load file
bpy.ops.wm.read_homefile(use_empty=True)
bpy.ops.wm.open_mainfile(filepath="{input_blend}")

print(f"Loaded {{len(bpy.data.objects)}} objects")

# Process each mesh object
processed = []
for obj in list(bpy.data.objects):
    if obj.type == 'MESH':
        print(f"Processing: {{obj.name}}, Scale: {{obj.scale[:]}}")

        # Get world matrix (includes scale, rotation, location)
        mat = obj.matrix_world.copy()
        mesh = obj.data
        new_mesh = mesh.copy()
        new_mesh.name = obj.name + "_baked"

        # Bake transforms into vertices
        for v in new_mesh.vertices:
            v.co = mat @ v.co

        # Create new object with transformed mesh
        new_obj = bpy.data.objects.new(obj.name + "_export", new_mesh)
        bpy.context.collection.objects.link(new_obj)

        # Remove old object
        bpy.data.objects.remove(obj)

        # Report bounds
        xs = [v.co.x for v in new_mesh.vertices]
        ys = [v.co.y for v in new_mesh.vertices]
        zs = [v.co.z for v in new_mesh.vertices]
        print(f"  Baked bounds: X({{min(xs):.1f}} to {{max(xs):.1f}}), Y({{min(ys):.1f}} to {{max(ys):.1f}}), Z({{min(zs):.1f}} to {{max(zs):.1f}})")
        processed.append(new_obj.name)

print(f"Processed {{len(processed)}} mesh(es)")

# Export to GLB
bpy.ops.export_scene.gltf(
    filepath="{output_glb}",
    export_format='GLB',
    export_yup={str(export_yup)},
    export_skins=False,
    export_animations=False
)

# Report file size
import os
if os.path.exists("{output_glb}"):
    size_kb = os.path.getsize("{output_glb}") / 1024
    print(f"Exported to: {output_glb}")
    print(f"File size: {{size_kb:.1f}} KB")

    # Verify GLB bounds
    import struct
    import json
    with open("{output_glb}", 'rb') as f:
        f.read(12)
        chunk_length = struct.unpack('<I', f.read(4))[0]
        f.read(4)
        gltf = json.loads(f.read(chunk_length).decode('utf-8'))
        for acc in gltf.get('accessors', []):
            if acc.get('type') == 'VEC3' and 'min' in acc:
                print(f"GLB bounds: {{acc['min']}} to {{acc['max']}}")
                break
else:
    print("ERROR: Export failed")
"""

    try:
        stdout, stderr, returncode = execute_blender_script(script, background=True)

        # Build result
        result = ""
        if returncode == 0:
            result += "✓ GLB export (baked) successful\n\n"
        else:
            result += f"⚠️  Export failed with code {returncode}\n\n"

        # Filter output
        filtered_lines = []
        for line in stdout.split('\n'):
            if any(skip in line for skip in [
                'Blender',
                'Read prefs:',
                'found bundled python:',
                'Read blend:',
                'Saved session',
                'Blender quit',
                'color management',
                'libpng warning'
            ]):
                continue
            if line.strip():
                filtered_lines.append(line)

        if filtered_lines:
            result += '\n'.join(filtered_lines)

        if stderr and returncode != 0:
            filtered_stderr = [l for l in stderr.split('\n')
                            if l.strip() and not any(s in l for s in ['color management', 'ALSA', 'OSL', 'libpng'])]
            if filtered_stderr:
                result += "\n\nErrors:\n" + '\n'.join(filtered_stderr)

        logger.info(f"Baked export completed with code {returncode}")
        return result

    except Exception as e:
        error_msg = f"Error exporting to GLB: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return error_msg


def main():
    """Start the MCP server on stdio transport."""
    logger.info("Blender MCP Server starting")
    logger.info(f"Blender path: {BLENDER_PATH}")

    # Verify Blender is available
    if not os.path.exists(BLENDER_PATH):
        logger.warning(f"Blender not found at {BLENDER_PATH}")
        logger.warning("Set BLENDER_PATH environment variable if Blender is installed elsewhere")

    # Run server with stdio transport (Claude Code manages the process)
    mcp.run(transport="stdio")


if __name__ == "__main__":
    main()
