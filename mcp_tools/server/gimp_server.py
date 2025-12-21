"""
MCP Server for GIMP Integration

This server exposes GIMP Python-Fu API tools to Claude Code.
It allows executing GIMP scripts and commands through the Python-Fu interface.
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
logger = logging.getLogger("gimp-server")

from fastmcp import FastMCP

# Initialize MCP server
mcp = FastMCP("gimp")

# GIMP executable path - configured via environment variable
GIMP_PATH = os.getenv('GIMP_PATH', '/opt/homebrew/bin/gimp')


def execute_gimp_script(script_content: str) -> tuple[str, str, int]:
    """
    Execute a Python script in GIMP.

    Args:
        script_content: Python script to execute in GIMP

    Returns:
        tuple: (stdout, stderr, return_code)
    """
    # Create temporary script file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.py', delete=False) as f:
        f.write(script_content)
        script_path = f.name

    try:
        # Build GIMP command - using batch mode
        cmd = [
            GIMP_PATH,
            '-i',  # No interface
            '-b', f'(python-fu-eval RUN-NONINTERACTIVE "{script_path}")',
            '-b', '(gimp-quit 0)'
        ]

        # Execute GIMP
        logger.info(f"Executing GIMP script")
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=120  # 120 second timeout
        )

        return result.stdout, result.stderr, result.returncode

    finally:
        # Cleanup temp file
        try:
            os.unlink(script_path)
        except Exception as e:
            logger.warning(f"Failed to delete temp file {script_path}: {e}")


def execute_gimp_python_simple(code: str) -> tuple[str, str, int]:
    """
    Execute Python code in GIMP using console mode.

    Args:
        code: Python code to execute

    Returns:
        tuple: (stdout, stderr, return_code)
    """
    # Wrap code in a complete script
    full_script = f"""#!/usr/bin/env python3
import sys
sys.path.insert(0, '/opt/homebrew/lib/gimp/2.0/python')

try:
    from gimpfu import *
    {code}
except Exception as e:
    print(f"Error: {{e}}")
    import traceback
    traceback.print_exc()
"""

    # Create temporary script file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.py', delete=False) as f:
        f.write(full_script)
        script_path = f.name
        os.chmod(script_path, 0o755)

    try:
        # Execute with gimp in console mode
        cmd = [GIMP_PATH, '-i', '-b', '-', '--batch-interpreter', 'python-fu-eval']

        logger.info(f"Executing GIMP Python code")
        result = subprocess.run(
            cmd,
            input=code,
            capture_output=True,
            text=True,
            timeout=120
        )

        return result.stdout, result.stderr, result.returncode

    finally:
        try:
            os.unlink(script_path)
        except:
            pass


@mcp.tool()
def execute_gimp_python(code: str) -> str:
    """
    Execute Python code in GIMP.

    Runs the provided Python code in a GIMP instance with access to the GIMP Python-Fu API.
    The code is executed in batch mode (no GUI).

    Args:
        code: Python code to execute (can use gimp, pdb, gimpfu modules)

    Returns:
        str: Output from the script execution

    Examples:
        - Get GIMP version: "import gimp; print(gimp.version)"
        - Create new image: "from gimpfu import *; img = pdb.gimp_image_new(800, 600, RGB)"
        - List brushes: "from gimpfu import *; print(pdb.gimp_brushes_get_list(''))"
    """
    logger.info(f"Executing GIMP Python code")

    try:
        stdout, stderr, returncode = execute_gimp_python_simple(code)

        # Build result message
        result = ""

        if returncode == 0:
            result += "✓ Script executed successfully\n\n"
        else:
            result += f"⚠️  Script exited with code {returncode}\n\n"

        # Filter output
        if stdout and stdout.strip():
            result += "Output:\n"
            result += stdout
            result += "\n"

        # Show stderr if there are errors/warnings
        if stderr:
            filtered_stderr = []
            for line in stderr.split('\n'):
                if any(skip in line.lower() for skip in [
                    'color management',
                    'gtk-warning',
                    'failed to load',
                    'libpng warning'
                ]):
                    continue
                if line.strip():
                    filtered_stderr.append(line)

            if filtered_stderr:
                result += "\nWarnings/Errors:\n"
                result += '\n'.join(filtered_stderr)
                result += "\n"

        logger.info(f"GIMP execution completed with code {returncode}")
        return result.strip() if result.strip() else "Script executed (no output)"

    except subprocess.TimeoutExpired:
        error_msg = "Error: Script execution timed out (120 second limit)"
        logger.error(error_msg)
        return error_msg
    except FileNotFoundError:
        error_msg = f"Error: GIMP not found at {GIMP_PATH}\nSet GIMP_PATH environment variable to the correct path."
        logger.error(error_msg)
        return error_msg
    except Exception as e:
        error_msg = f"Error executing GIMP script: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return error_msg


@mcp.tool()
def create_gimp_image(width: int, height: int, image_type: str = "RGB", filepath: str = None) -> str:
    """
    Create a new image in GIMP.

    Args:
        width: Image width in pixels
        height: Image height in pixels
        image_type: Image type (RGB, GRAY, INDEXED)
        filepath: Optional path to save the image

    Returns:
        str: Result of image creation
    """
    logger.info(f"Creating GIMP image {width}x{height} ({image_type})")

    type_map = {
        'RGB': 0,
        'GRAY': 1,
        'INDEXED': 2
    }

    img_type = type_map.get(image_type.upper(), 0)

    script = f"""
from gimpfu import *

# Create new image
img = pdb.gimp_image_new({width}, {height}, {img_type})

# Create a layer
layer = pdb.gimp_layer_new(img, {width}, {height}, {img_type}, "Background", 100, 0)

# Add layer to image
pdb.gimp_image_insert_layer(img, layer, None, 0)

# Fill with white
pdb.gimp_context_set_foreground((255, 255, 255))
pdb.gimp_drawable_fill(layer, 0)

print(f"Created {{img.width}}x{{img.height}} image")
"""

    if filepath:
        filepath = os.path.abspath(filepath)
        os.makedirs(os.path.dirname(filepath), exist_ok=True)
        script += f"""
# Save image
pdb.gimp_file_save(img, layer, "{filepath}", "{filepath}")
print(f"Saved to: {filepath}")
"""

    script += """
# Clean up
pdb.gimp_image_delete(img)
"""

    return execute_gimp_python(script)


@mcp.tool()
def get_gimp_info() -> str:
    """
    Get information about the GIMP installation.

    Returns:
        str: GIMP version and system information
    """
    logger.info("Getting GIMP info")

    script = """
import gimp
print(f"GIMP Version: {'.'.join(map(str, gimp.version))}")
"""

    return execute_gimp_python(script)


@mcp.tool()
def apply_gimp_filter(input_file: str, output_file: str, filter_name: str, **params) -> str:
    """
    Apply a filter to an image file.

    Args:
        input_file: Path to input image
        output_file: Path to save filtered image
        filter_name: Name of filter (blur, sharpen, edge-detect, etc.)
        params: Additional filter parameters

    Returns:
        str: Result of filter operation
    """
    input_file = os.path.abspath(input_file)
    output_file = os.path.abspath(output_file)

    if not os.path.exists(input_file):
        return f"Error: Input file not found: {input_file}"

    logger.info(f"Applying filter '{filter_name}' to {input_file}")

    script = f"""
from gimpfu import *

# Load image
img = pdb.gimp_file_load("{input_file}", "{input_file}")
layer = pdb.gimp_image_get_active_layer(img)

# Apply filter (example with blur)
pdb.plug_in_gauss({filter_name})

# Save result
pdb.gimp_file_save(img, layer, "{output_file}", "{output_file}")

print(f"Saved filtered image to: {output_file}")

# Clean up
pdb.gimp_image_delete(img)
"""

    return execute_gimp_python(script)


def main():
    """Start the MCP server on stdio transport."""
    logger.info("GIMP MCP Server starting")
    logger.info(f"GIMP path: {GIMP_PATH}")

    # Verify GIMP is available
    if not os.path.exists(GIMP_PATH):
        logger.warning(f"GIMP not found at {GIMP_PATH}")
        logger.warning("Set GIMP_PATH environment variable if GIMP is installed elsewhere")

    # Run server with stdio transport (Claude Code manages the process)
    mcp.run(transport="stdio")


if __name__ == "__main__":
    main()
