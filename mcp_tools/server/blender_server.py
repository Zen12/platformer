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
def retarget_mixamo_animation(input_fbx: str, output_glb: str) -> str:
    """
    Retarget Mixamo animation (65 bones) to game engine skeleton (18 bones).

    This tool converts Mixamo animations to a simplified 18-bone skeleton compatible
    with the game engine's Ozz animation system. It deletes 48 bones (fingers, toes,
    extra spine bones), renames the remaining 17 bones, adds a Root bone, and aligns
    the rest pose to origin.

    Args:
        input_fbx: Path to Mixamo FBX animation file (65 bones)
        output_glb: Path to save retargeted GLB file (18 bones)

    Returns:
        str: Result of retargeting operation with bone count and file size

    Example:
        retarget_mixamo_animation(
            "assets/resources/models/shooter/Idle.fbx",
            "assets/resources/models/shooter/idle_18bones.glb"
        )

    Post-processing:
        After retargeting, run: python3 mcp_tools/server/generate_asset_metadata.py
    """
    logger.info(f"Retargeting Mixamo animation: {input_fbx} -> {output_glb}")

    # Make paths absolute
    input_fbx = os.path.abspath(input_fbx)
    output_glb = os.path.abspath(output_glb)

    # Verify input exists
    if not os.path.exists(input_fbx):
        return f"Error: Input FBX file not found: {input_fbx}"

    # Ensure output directory exists
    os.makedirs(os.path.dirname(output_glb), exist_ok=True)

    # Build retargeting script (inline to avoid import issues in Blender)
    script = f"""import bpy
import sys
from mathutils import Vector

def retarget_mixamo_animation(input_fbx_path, output_glb_path):
    # Clear default scene
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete()

    # Import Mixamo FBX
    print(f"Importing {{input_fbx_path}}...")
    bpy.ops.import_scene.fbx(filepath=input_fbx_path)

    # Find armature
    armature_obj = None
    for obj in bpy.data.objects:
        if obj.type == 'ARMATURE':
            armature_obj = obj
            break

    if not armature_obj:
        print("ERROR: No armature found in FBX file")
        return False

    armature = armature_obj.data
    print(f"Found armature: {{armature_obj.name}}")

    # Define bone mapping (17 Mixamo bones to keep)
    bones_to_keep = {{
        "mixamorig:Hips": "Hips",
        "mixamorig:Spine": "Spine",
        "mixamorig:Spine1": "Chest",
        "mixamorig:Neck": "Neck",
        "mixamorig:Head": "Head",
        "mixamorig:LeftArm": "UpperArm.L",
        "mixamorig:LeftForeArm": "Forearm.L",
        "mixamorig:LeftHand": "Hand.L",
        "mixamorig:RightArm": "UpperArm.R",
        "mixamorig:RightForeArm": "Forearm.R",
        "mixamorig:RightHand": "Hand.R",
        "mixamorig:LeftUpLeg": "Thigh.L",
        "mixamorig:LeftLeg": "Shin.L",
        "mixamorig:LeftFoot": "Foot.L",
        "mixamorig:RightUpLeg": "Thigh.R",
        "mixamorig:RightLeg": "Shin.R",
        "mixamorig:RightFoot": "Foot.R",
    }}

    # Switch to edit mode
    bpy.context.view_layer.objects.active = armature_obj
    bpy.ops.object.mode_set(mode='EDIT')

    # Get Hips position before modifications
    hips_bone = armature.edit_bones.get("mixamorig:Hips")
    if hips_bone:
        hips_y_pos = hips_bone.head.y
        print(f"Original Hips Y position: {{hips_y_pos}}")
    else:
        print("WARNING: Hips bone not found")
        hips_y_pos = 0

    # Delete bones not in the mapping
    bones_to_delete = [bone.name for bone in armature.edit_bones
                      if bone.name not in bones_to_keep]
    print(f"Deleting {{len(bones_to_delete)}} bones...")
    for bone_name in bones_to_delete:
        armature.edit_bones.remove(armature.edit_bones[bone_name])

    # Rename kept bones
    for old_name, new_name in bones_to_keep.items():
        if old_name in armature.edit_bones:
            armature.edit_bones[old_name].name = new_name

    # Add Root bone at origin with Hips as child
    root_bone = armature.edit_bones.new("Root")
    hips_bone = armature.edit_bones.get("Hips")
    if hips_bone:
        root_bone.head = Vector((0, 0, 0))
        root_bone.tail = Vector((0, 0, 0.1))
        hips_bone.parent = root_bone
        print("Created Root bone")
    else:
        print("ERROR: Could not find Hips bone for reparenting")
        return False

    # Switch to object mode
    bpy.ops.object.mode_set(mode='OBJECT')

    # CRITICAL: Offset armature to align Root at world origin
    print(f"Original armature location: {{armature_obj.location}}")
    armature_obj.location.y -= hips_y_pos
    print(f"New armature location: {{armature_obj.location}}")

    # Apply transform to bake offset into skeleton
    bpy.ops.object.select_all(action='DESELECT')
    armature_obj.select_set(True)
    bpy.context.view_layer.objects.active = armature_obj
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    print("Applied transform to armature")

    # Verify final bone count
    bone_count = len(armature.bones)
    print(f"Final bone count: {{bone_count}}")

    # Export to GLB
    print("Exporting to GLB...")
    bpy.ops.export_scene.gltf(
        filepath=output_glb_path,
        export_format='GLB',
        use_selection=False,
        export_animations=True,
        export_frame_range=True,
        export_force_sampling=True,
        export_def_bones=True,
        export_rest_position_armature=False
    )
    print("Export complete")

    # Check file size
    import os
    if os.path.exists(output_glb_path):
        size_kb = os.path.getsize(output_glb_path) / 1024
        print(f"File size: {{size_kb:.1f}} KB")
        return True
    else:
        print("ERROR: Export failed")
        return False

# Run retargeting
success = retarget_mixamo_animation("{input_fbx}", "{output_glb}")
sys.exit(0 if success else 1)
"""

    try:
        stdout, stderr, returncode = execute_blender_script(script, background=True)

        # Build result
        result = ""
        if returncode == 0:
            result += "✓ Animation retargeted successfully\n\n"
        else:
            result += f"⚠️  Retargeting failed with code {{returncode}}\n\n"

        # Filter output
        filtered_lines = []
        for line in stdout.split('\n'):
            if any(skip in line for skip in ['Blender', 'Read prefs:', 'found bundled python:', 'Read blend:', 'Saved session', 'Blender quit']):
                continue
            if line.strip():
                filtered_lines.append(line)

        if filtered_lines:
            result += '\n'.join(filtered_lines)
        else:
            result += "No output"

        if stderr and returncode != 0:
            result += "\n\nErrors:\n" + stderr

        logger.info(f"Retargeting completed with code {{returncode}}")
        return result

    except Exception as e:
        error_msg = f"Error retargeting animation: {{str(e)}}"
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
