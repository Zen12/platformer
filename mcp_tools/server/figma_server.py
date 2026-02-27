"""
MCP Server for Figma-to-RML Conversion

Exposes Figma conversion tools to Claude Code.
Follows the same pattern as asset_server.py.
"""

import os
import sys
import logging

# Add parent directory (mcp_tools/) to path so we can import shared modules
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

# Setup logging to stderr (NEVER use stdout with stdio transport)
logging.basicConfig(
    stream=sys.stderr,
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger("figma-server")

from fastmcp import FastMCP
from figma_to_rml import convert_frame, list_figma_frames_api

# Initialize MCP server
mcp = FastMCP("figma-to-rml")


@mcp.tool()
def figma_to_rml(
    file_key: str,
    page_name: str,
    frame_name: str,
    token: str = "",
    config_path: str = "",
) -> str:
    """
    Convert a Figma frame to RmlUi assets (.rml, .css, .upage with .meta files).

    Fetches the specified frame from a Figma file, generates RML markup and CSS
    styles, and writes all asset files to assets/resources/ui/<page_name>/.

    Args:
        file_key: Figma file key (the part after /file/ in the Figma URL)
        page_name: Output page name (used for directory and file names, e.g., "main_menu")
        frame_name: Name of the Figma frame to convert (e.g., "Main Menu")
        token: Figma API token (optional, reads FIGMA_TOKEN env var if empty)
        config_path: Path to figma_naming.yaml for custom mappings (optional)

    Returns:
        str: Summary of created files with UPAGE GUID for FSM integration
    """
    logger.info(f"Converting Figma frame '{frame_name}' to page '{page_name}'")

    try:
        result = convert_frame(
            file_key=file_key,
            frame_name=frame_name,
            page_name=page_name,
            token=token or None,
            config_path=config_path or None,
            write_files=True,
        )

        output = f"Conversion complete!\n\n"
        output += f"Output directory: {result['dir']}\n"
        output += f"Files created:\n"
        output += f"  - {result['rml_path']}\n"
        output += f"  - {result['rml_path']}.meta\n"
        output += f"  - {result['css_path']}\n"
        output += f"  - {result['css_path']}.meta\n"
        output += f"  - {result['upage_path']}\n"
        output += f"  - {result['upage_path']}.meta\n\n"
        output += f"UPAGE GUID: {result['upage_guid']}\n"
        output += f"Use this GUID in your FSM to load the UI page.\n"

        if result.get("template_guids"):
            output += f"\nTemplate GUIDs used: {', '.join(result['template_guids'])}\n"

        logger.info(f"Conversion successful: {result['dir']}")
        return output

    except Exception as e:
        error_msg = f"Error converting Figma frame: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return f"Error: {error_msg}"


@mcp.tool()
def list_figma_frames(
    file_key: str,
    token: str = "",
) -> str:
    """
    List all pages and top-level frames in a Figma file.

    Use this to discover which frames are available for conversion.

    Args:
        file_key: Figma file key (the part after /file/ in the Figma URL)
        token: Figma API token (optional, reads FIGMA_TOKEN env var if empty)

    Returns:
        str: List of pages and frames in the Figma file
    """
    logger.info(f"Listing frames for Figma file: {file_key}")

    try:
        frames = list_figma_frames_api(file_key, token=token or None)

        if not frames:
            return "No frames found in the Figma file."

        output = f"Found {len(frames)} frame(s):\n\n"
        current_page = None
        for page_name, frame_name in frames:
            if page_name != current_page:
                output += f"  Page: {page_name}\n"
                current_page = page_name
            output += f"    - {frame_name}\n"

        logger.info(f"Found {len(frames)} frames")
        return output

    except Exception as e:
        error_msg = f"Error listing Figma frames: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return f"Error: {error_msg}"


@mcp.tool()
def preview_figma_conversion(
    file_key: str,
    page_name: str,
    frame_name: str,
    token: str = "",
    config_path: str = "",
) -> str:
    """
    Preview RML/CSS generation without writing files.

    Dry-run of the conversion pipeline. Returns the generated RML and CSS
    content so you can review before committing to disk.

    Args:
        file_key: Figma file key (the part after /file/ in the Figma URL)
        page_name: Output page name (e.g., "main_menu")
        frame_name: Name of the Figma frame to convert (e.g., "Main Menu")
        token: Figma API token (optional, reads FIGMA_TOKEN env var if empty)
        config_path: Path to figma_naming.yaml for custom mappings (optional)

    Returns:
        str: Generated RML and CSS content for review
    """
    logger.info(f"Previewing conversion of Figma frame '{frame_name}'")

    try:
        result = convert_frame(
            file_key=file_key,
            frame_name=frame_name,
            page_name=page_name,
            token=token or None,
            config_path=config_path or None,
            write_files=False,
        )

        output = f"{'=' * 60}\n"
        output += f"  RML Preview: {page_name}.rml\n"
        output += f"{'=' * 60}\n"
        output += result["rml_content"] + "\n\n"
        output += f"{'=' * 60}\n"
        output += f"  CSS Preview: {page_name}.css\n"
        output += f"{'=' * 60}\n"
        output += result["css_content"] + "\n"

        if result.get("template_guids"):
            output += f"\nTemplate GUIDs required: {', '.join(result['template_guids'])}\n"

        logger.info("Preview generated successfully")
        return output

    except Exception as e:
        error_msg = f"Error previewing conversion: {str(e)}"
        logger.error(error_msg, exc_info=True)
        return f"Error: {error_msg}"


def main():
    """Start the MCP server on stdio transport."""
    logger.info("Figma-to-RML MCP Server starting")

    # Run server with stdio transport (Claude Code manages the process)
    mcp.run(transport="stdio")


if __name__ == "__main__":
    main()
