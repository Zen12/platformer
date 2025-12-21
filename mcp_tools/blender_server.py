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
