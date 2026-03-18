"""
MCP Server for Platformer Asset Management

This server exposes asset metadata tools to Claude Code.
It wraps the existing import.py and clear_all_refs.py utilities.
"""

import os
import sys
import logging
import uuid

# Add parent directory (mcp_tools/) to path so we can import shared modules
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))

# Setup logging to stderr (NEVER use stdout with stdio transport)
logging.basicConfig(
    stream=sys.stderr,
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger("asset-server")

from fastmcp import FastMCP
from asset_utils import dict_to_yaml, ext_check
from find import read_meta_file, find_guid_references
from create import create_project_yaml, create_fsm, create_shader, create_material

# Initialize MCP server
mcp = FastMCP("platformer-assets")

# Asset directory - configured via environment variable
ASSETS_DIR = os.getenv('ASSETS_DIR', 'assets/resources')


@mcp.tool()
def generate_asset_metadata() -> str:
    """
    Generate .meta files for all assets that don't have them.

    Scans the assets/resources directory and creates metadata files
    for any assets missing them (similar to Unity's asset system).
    Each .meta file contains a GUID, name, extension, and type.

    Returns:
        str: Summary of created metadata files
    """
    logger.info("Starting asset metadata generation")

    if not os.path.exists(ASSETS_DIR):
        error_msg = f"Assets directory not found: {ASSETS_DIR}"
        logger.error(error_msg)
        return f"Error: {error_msg}"

    created_files = []

    try:
        for root, dirs, files in os.walk(ASSETS_DIR):
            # Skip hidden folders and __pycache__
            dirs[:] = [d for d in dirs if not d.startswith('.') and d != '__pycache__']

            for file in files:
                # Skip hidden files and .meta files
                if file.startswith('.') or file.endswith('.meta'):
                    continue

                base_name, ext = os.path.splitext(file)
                meta_path = os.path.join(root, base_name + ext + '.meta')

                # Skip if .meta already exists
                if os.path.isfile(meta_path):
                    continue

                # Create metadata
                data = {
                    'name': base_name,
                    'guid': str(uuid.uuid4()),
                    'extension': ext,
                    'type': ext_check(ext)
                }

                # Write .meta file
                with open(meta_path, 'w') as f:
                    f.write(dict_to_yaml(data))

                created_files.append(meta_path)
                logger.info(f"Created: {meta_path}")

        if created_files:
            result = f"Successfully created {len(created_files)} metadata files:\n"
            for path in created_files:
                result += f"  - {path}\n"
            logger.info(f"Generated {len(created_files)} metadata files")
            return result
        else:
            logger.info("No new metadata files needed")
            return "All assets already have metadata files."

    except Exception as e:
        error_msg = f"Error generating metadata: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return f"Error: {error_msg}"


@mcp.tool()
def delete_all_asset_metadata() -> str:
    """
    Delete all .meta files from the assets directory.

    Removes all metadata files that were previously generated.
    Use this to clean up before regenerating metadata.

    Returns:
        str: Summary of deleted metadata files
    """
    logger.info("Starting asset metadata deletion")

    if not os.path.exists(ASSETS_DIR):
        error_msg = f"Assets directory not found: {ASSETS_DIR}"
        logger.error(error_msg)
        return f"Error: {error_msg}"

    deleted_files = []
    failed_files = []

    try:
        for root, dirs, files in os.walk(ASSETS_DIR):
            for file in files:
                if file.endswith('.meta'):
                    file_path = os.path.join(root, file)
                    try:
                        os.remove(file_path)
                        deleted_files.append(file_path)
                        logger.info(f"Deleted: {file_path}")
                    except Exception as e:
                        failed_files.append((file_path, str(e)))
                        logger.error(f"Failed to delete {file_path}: {e}")

        result = f"Deleted {len(deleted_files)} metadata files"
        if failed_files:
            result += f"\nFailed to delete {len(failed_files)} files:\n"
            for path, error in failed_files:
                result += f"  - {path}: {error}\n"
        elif deleted_files:
            result += ":\n"
            for path in deleted_files:
                result += f"  - {path}\n"
        else:
            result = "No metadata files found to delete."

        logger.info(f"Deleted {len(deleted_files)} metadata files")
        return result

    except Exception as e:
        error_msg = f"Error deleting metadata: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return f"Error: {error_msg}"


@mcp.tool()
def list_assets_without_metadata() -> str:
    """
    List all assets that don't have .meta files.

    Scans the assets directory and reports which files are missing metadata.
    Useful for checking what will be affected by generate_asset_metadata.

    Returns:
        str: List of assets without metadata files
    """
    logger.info("Listing assets without metadata")

    if not os.path.exists(ASSETS_DIR):
        error_msg = f"Assets directory not found: {ASSETS_DIR}"
        logger.error(error_msg)
        return f"Error: {error_msg}"

    missing_meta = []

    try:
        for root, dirs, files in os.walk(ASSETS_DIR):
            dirs[:] = [d for d in dirs if not d.startswith('.') and d != '__pycache__']

            for file in files:
                if file.startswith('.') or file.endswith('.meta'):
                    continue

                base_name, ext = os.path.splitext(file)
                meta_path = os.path.join(root, base_name + ext + '.meta')

                if not os.path.isfile(meta_path):
                    asset_path = os.path.join(root, file)
                    asset_type = ext_check(ext)
                    missing_meta.append((asset_path, asset_type))

        if missing_meta:
            result = f"Found {len(missing_meta)} assets without metadata:\n"
            for path, asset_type in missing_meta:
                result += f"  - {path} ({asset_type})\n"
        else:
            result = "All assets have metadata files."

        logger.info(f"Found {len(missing_meta)} assets without metadata")
        return result

    except Exception as e:
        error_msg = f"Error listing assets: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return f"Error: {error_msg}"


@mcp.tool()
def delete_asset(file_path: str) -> str:
    """
    Delete an asset file and its .meta file, with reference checking.

    Checks if the asset is referenced by other assets before deletion.
    If references are found, returns a warning message without deleting.
    If no references are found, or force=true, deletes both files.

    Args:
        file_path: Path to the asset file to delete

    Returns:
        str: Result message indicating success, failure, or warnings about references
    """
    logger.info(f"Delete request for: {file_path}")

    if not os.path.isfile(file_path):
        error_msg = f"File not found: {file_path}"
        logger.error(error_msg)
        return f"Error: {error_msg}"

    meta_path = file_path + '.meta'
    has_meta = os.path.isfile(meta_path)

    if not has_meta:
        logger.warning(f"No .meta file found for {file_path}")
        result = f"Warning: No .meta file found at {meta_path}\n"
        result += "This file may not be a tracked asset.\n"
        result += "Use the delete.py script directly if you want to delete this file."
        return result

    guid = read_meta_file(meta_path)
    if not guid:
        logger.warning(f"Could not read GUID from {meta_path}")
        result = "Warning: Could not read GUID from .meta file.\n"
        result += "Cannot check for references. Deletion aborted."
        return result

    logger.info(f"Asset GUID: {guid}")
    references = find_guid_references(guid)

    if references:
        result = f"⚠️  CANNOT DELETE: Found {len(references)} reference(s) to this asset:\n\n"
        for ref_path, line_num, line_content in references:
            result += f"  {ref_path}:{line_num}\n"
            result += f"    {line_content}\n\n"
        result += "Deleting this asset would break these references!\n"
        result += "Please remove all references first, or use the delete.py script with manual confirmation."
        logger.warning(f"Deletion blocked: {len(references)} references found")
        return result

    try:
        os.remove(file_path)
        logger.info(f"Deleted: {file_path}")
        result = f"Deleted: {file_path}\n"

        if has_meta:
            os.remove(meta_path)
            logger.info(f"Deleted: {meta_path}")
            result += f"Deleted: {meta_path}\n"

        result += "\n✓ Asset successfully deleted (no references found)."
        return result

    except Exception as e:
        error_msg = f"Error deleting files: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return f"Error: {error_msg}"


@mcp.tool()
def check_asset_references(file_path: str) -> str:
    """
    Check if an asset is referenced by other assets.

    Reads the asset's GUID from its .meta file and searches for
    references in all asset files (.scene, .prefab, .mat, .upage, .fsm).

    Args:
        file_path: Path to the asset file to check

    Returns:
        str: List of files that reference this asset, or confirmation that no references exist
    """
    logger.info(f"Checking references for: {file_path}")

    if not os.path.isfile(file_path):
        error_msg = f"File not found: {file_path}"
        logger.error(error_msg)
        return f"Error: {error_msg}"

    meta_path = file_path + '.meta'
    if not os.path.isfile(meta_path):
        return f"Warning: No .meta file found. Cannot check references."

    guid = read_meta_file(meta_path)
    if not guid:
        return "Error: Could not read GUID from .meta file."

    logger.info(f"Asset GUID: {guid}")
    references = find_guid_references(guid)

    if references:
        result = f"Found {len(references)} reference(s) to {file_path}:\n\n"
        for ref_path, line_num, line_content in references:
            result += f"  {ref_path}:{line_num}\n"
            result += f"    {line_content}\n\n"
        logger.info(f"Found {len(references)} references")
        return result
    else:
        result = f"✓ No references found for {file_path}\n"
        result += "This asset is safe to delete."
        logger.info("No references found")
        return result


@mcp.tool()
def create_project(
    name: str = "New Project",
    resolution_width: int = 1600,
    resolution_height: int = 1200,
    target_fps: int = 30,
    main_fsm_guid: str = None
) -> str:
    """
    Create a new project.yaml file with the specified configuration.

    Args:
        name: Project name (default: "New Project")
        resolution_width: Screen width in pixels (default: 1600)
        resolution_height: Screen height in pixels (default: 1200)
        target_fps: Target frames per second (default: 30)
        main_fsm_guid: GUID of the main FSM, or None to generate a new one

    Returns:
        str: Summary of created project file
    """
    logger.info(f"Creating project: {name}")

    # Get base assets directory (parent of ASSETS_DIR)
    # If ASSETS_DIR is "assets/resources", we want "assets"
    assets_base = os.path.dirname(ASSETS_DIR) if ASSETS_DIR.endswith('resources') else ASSETS_DIR
    output_path = os.path.join(assets_base, 'project.yaml')

    try:
        created_path = create_project_yaml(
            name=name,
            resolution_width=resolution_width,
            resolution_height=resolution_height,
            target_fps=target_fps,
            main_fsm_guid=main_fsm_guid,
            output_path=output_path
        )

        result = f"✓ Created project file: {created_path}\n\n"
        result += "Project configuration:\n"
        result += f"  Name: {name}\n"
        result += f"  Resolution: {resolution_width}x{resolution_height}\n"
        result += f"  Target FPS: {target_fps}\n"

        # Read back the file to get the actual GUID
        with open(created_path, 'r') as f:
            for line in f:
                if line.startswith('main_fsm:'):
                    guid = line.split(':', 1)[1].strip()
                    result += f"  Main FSM GUID: {guid}\n"
                    break

        logger.info(f"Project created successfully: {created_path}")
        return result

    except Exception as e:
        error_msg = f"Error creating project: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return f"Error: {error_msg}"


@mcp.tool()
def create_shader_files(name: str = "new_shader") -> str:
    """
    Create a new shader with OpenGL and GLES versions.

    Creates a shader folder containing:
    - gl.vert and gl_.frag (OpenGL shaders)
    - gles.vert and gles_.frag (GLES shaders)
    - .meta files for all shader files
    - Fragment shaders use underscore for alphabetical ordering
    - Each shader file has its own unique GUID

    Args:
        name: Shader name (used for folder name)

    Returns:
        str: Summary of created shader files
    """
    logger.info(f"Creating shader: {name}")

    try:
        folder_path, gl_vert_guid, gl_frag_guid, gles_vert_guid, gles_frag_guid, files = create_shader(name=name)

        result = f"✓ Created shader folder: {folder_path}\n\n"
        result += "Files created:\n"
        for file in files:
            result += f"  - {file}\n"
        result += "\n"
        result += "Shader configuration:\n"
        result += f"  Name: {name}\n"
        result += f"  GL Vertex Shader GUID: {gl_vert_guid}\n"
        result += f"  GL Fragment Shader GUID: {gl_frag_guid}\n"
        result += f"  GLES Vertex Shader GUID: {gles_vert_guid}\n"
        result += f"  GLES Fragment Shader GUID: {gles_frag_guid}\n"

        logger.info(f"Shader created successfully: {folder_path}")
        return result

    except Exception as e:
        error_msg = f"Error creating shader: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return f"Error: {error_msg}"


@mcp.tool()
def create_fsm_file(name: str = "new_fsm") -> str:
    """
    Create a new FSM file with default structure.

    Creates an FSM with:
    - 2 nodes (Start Node and Second Node)
    - Each node has a log action
    - 1 trigger condition (continue_trigger)
    - 1 connection between the nodes
    - Automatic .meta file generation

    Args:
        name: FSM name (used for filename and internal name)

    Returns:
        str: Summary of created FSM files
    """
    logger.info(f"Creating FSM: {name}")

    try:
        fsm_path, meta_path, fsm_guid = create_fsm(name=name)

        result = f"✓ Created FSM file: {fsm_path}\n"
        result += f"✓ Created metadata: {meta_path}\n\n"
        result += "FSM configuration:\n"
        result += f"  Name: {name}\n"
        result += f"  GUID: {fsm_guid}\n"
        result += f"  Nodes: 2\n"
        result += f"    - Start Node (log: 'FSM Started')\n"
        result += f"    - Second Node (log: 'Second Node Reached')\n"
        result += f"  Conditions: 1 (trigger_check: continue_trigger)\n"
        result += f"  Connections: 1 (Start Node → Second Node)\n"

        logger.info(f"FSM created successfully: {fsm_path}")
        return result

    except Exception as e:
        error_msg = f"Error creating FSM: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return f"Error: {error_msg}"


@mcp.tool()
def create_material_file(
    name: str = "new_material",
    opengl_vertex_guid: str = None,
    opengl_fragment_guid: str = None,
    gles_vertex_guid: str = None,
    gles_fragment_guid: str = None,
    image_guid: str = None,
    font_guid: str = None,
    blend_mode: int = 0,
    is_culling: bool = False
) -> str:
    """
    Create or update a material file (.mat) with shader references.

    Materials link together shaders (vertex/fragment for both OpenGL and GLES)
    and optionally reference textures and fonts. If a material with the same
    name already exists, it will be updated while preserving its GUID.

    Args:
        name: Material name (used for filename)
        opengl_vertex_guid: GUID of OpenGL vertex shader
        opengl_fragment_guid: GUID of OpenGL fragment shader
        gles_vertex_guid: GUID of GLES vertex shader
        gles_fragment_guid: GUID of GLES fragment shader
        image_guid: Optional GUID of texture image
        font_guid: Optional GUID of font
        blend_mode: Blend mode (0=none, 1=alpha blending, etc.)
        is_culling: Enable face culling

    Returns:
        str: Summary of created/updated material file
    """
    logger.info(f"Creating material: {name}")

    try:
        mat_path, meta_path, mat_guid = create_material(
            name=name,
            opengl_vertex_guid=opengl_vertex_guid,
            opengl_fragment_guid=opengl_fragment_guid,
            gles_vertex_guid=gles_vertex_guid,
            gles_fragment_guid=gles_fragment_guid,
            image_guid=image_guid,
            font_guid=font_guid,
            blend_mode=blend_mode,
            is_culling=is_culling
        )

        result = f"✓ Created material file: {mat_path}\n"
        result += f"✓ Created metadata: {meta_path}\n\n"
        result += "Material configuration:\n"
        result += f"  Name: {name}\n"
        result += f"  GUID: {mat_guid}\n"
        result += f"  Blend Mode: {blend_mode}\n"
        result += f"  Culling: {is_culling}\n"

        if opengl_vertex_guid and opengl_fragment_guid:
            result += f"  OpenGL Shaders:\n"
            result += f"    Vertex: {opengl_vertex_guid}\n"
            result += f"    Fragment: {opengl_fragment_guid}\n"

        if gles_vertex_guid and gles_fragment_guid:
            result += f"  GLES Shaders:\n"
            result += f"    Vertex: {gles_vertex_guid}\n"
            result += f"    Fragment: {gles_fragment_guid}\n"

        if image_guid:
            result += f"  Image: {image_guid}\n"

        if font_guid:
            result += f"  Font: {font_guid}\n"

        logger.info(f"Material created successfully: {mat_path}")
        return result

    except Exception as e:
        error_msg = f"Error creating material: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return f"Error: {error_msg}"


def main():
    """Start the MCP server on stdio transport."""
    logger.info("Platformer Asset MCP Server starting")
    logger.info(f"Assets directory: {ASSETS_DIR}")

    # Run server with stdio transport (Claude Code manages the process)
    mcp.run(transport="stdio")


if __name__ == "__main__":
    main()