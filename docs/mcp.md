# 🤖 MCP Setup (Model Context Protocol)

The MCP server provides AI-assisted asset management for the platformer project.

## Architecture

The project uses a **two-repository structure**:
1. **Logic project** (this repo): C++/CMake/Python scripts
2. **Assets project** (separate): YAML/.png/game assets

The MCP server manages assets in the assets repository only.

## Prerequisites

- Python 3.7+
- Claude Code CLI

## One-Command Setup & Launch

The `run_claude.sh` script handles everything automatically:
-  Creates Python virtual environment (if needed)
-  Installs MCP SDK (if needed)
-  Configures environment variables
-  Launches Claude Code

```bash
# Use default assets directory (./assets/resources)
./run_claude.sh

# Or specify custom assets directory
./run_claude.sh /path/to/game-assets

# Or set environment variable
export ASSETS_DIR=/path/to/game-assets
./run_claude.sh
```

**Note:** The MCP server will take a few seconds to initialize on first launch.

**That's it!** The script will handle all setup on first run.

## Available MCP Tools

### Asset Management Tools

Once configured, Claude Code can help you with:
- **Create Project** - Generate a new project.yaml file with configuration
- **Create FSM** - Generate a new FSM file with default structure and metadata
- **Create Shader** - Generate shader folder with GL and GLES versions (unique GUIDs)
- **Generate Asset Metadata** - Create .meta files for assets without them
- **Delete Asset Metadata** - Remove all .meta files
- **List Assets Without Metadata** - Check which assets need metadata
- **Check Asset References** - Find all files that reference a specific asset
- **Delete Asset** - Safely delete assets with reference checking
- **GUID-based Asset Loading** - Use `guid://` protocol in RML/CSS files

### Blender Tools

The Blender MCP server provides:
- **Execute Blender Python** - Run arbitrary Python code in Blender (bpy, bmesh, mathutils)
- **Create Blender Object** - Create basic 3D objects (cube, sphere, cylinder, etc.)
- **Save Blender File** - Save scenes to .blend files
- **Export to GLB** - Export .blend files to GLB format for the game engine
- **Get Blender Info** - View Blender version and system information
- **List Blender Objects** - List all objects in a scene or .blend file

## Usage Examples

### In Claude Code

#### Asset Management
```
- "Create a new project called 'My Game' with 1920x1080 resolution"
- "Create a new FSM called 'menu_fsm'"
- "Create a new shader called 'custom_shader'"
- "Generate metadata for all assets"
- "List assets without metadata"
- "Find all references to awesomeface.png"
- "Delete sprite2.mat"
```

#### Blender
```
- "Create a cube in Blender at position (0, 0, 5)"
- "Execute Python code in Blender to create a sphere"
- "List all objects in the Blender scene"
- "Save the Blender scene to assets/models/my_scene.blend"
- "Get Blender version information"
- "Create a monkey head mesh and save it as monkey.blend"
- "Export my_model.blend to GLB format"
```

### Command Line

```bash
# Create a new project
python3 mcp_tools/create.py project --name "My Game" --resolution 1920 1080 --fps 60

# Create a new FSM
python3 mcp_tools/create.py fsm --name menu_fsm

# Create a new shader (GL + GLES with unique GUIDs)
python3 mcp_tools/create.py shader --name custom_shader

# Find all references to an asset
python3 mcp_tools/find.py assets/resources/images/player.png

# Find references by GUID
python3 mcp_tools/find.py --guid f22681f4-45d1-4542-a838-99f854890c18

# Import/create metadata for new assets
python3 mcp_tools/import.py                          # All assets
python3 mcp_tools/import.py assets/resources/images  # Specific folder
python3 mcp_tools/import.py assets/resources/test.png # Specific file

# Delete assets with reference checking
python3 mcp_tools/delete.py assets/resources/test.png
```

## GUID-Based Asset References

All asset types now support GUID-based references for rename-safe asset management:

### RML/CSS Files

```rml
<img src="guid://126fb991-fe1c-45e0-ae0c-1649a2c65ae3" width="100" height="100"/>
```

### Material Files

```yaml
image: 126fb991-fe1c-45e0-ae0c-1649a2c65ae3
```

### Scene/Prefab Files

```yaml
material: f22681f4-45d1-4542-a838-99f854890c18
```

## Configuration

The `.mcp.json` configuration uses `${PROJECT_ROOT}` for portability across different machines.

## MCP Server Files

### Asset Management
- `mcp_tools/asset_server.py` - Main MCP server implementation
- `mcp_tools/asset_utils.py` - Shared utilities for asset management
- `mcp_tools/create.py` - Project and asset creation utilities
- `mcp_tools/find.py` - GUID reference finder
- `mcp_tools/import.py` - Asset metadata generator
- `mcp_tools/delete.py` - Safe asset deletion with reference checking

### Blender Integration
- `mcp_tools/server/blender_server.py` - Blender MCP server for 3D modeling and automation
- `mcp_tools/export_to_glb.py` - Standalone Blender to GLB export script

### GIMP Integration
- `mcp_tools/server/gimp_server.py` - GIMP MCP server for 2D image creation and editing

## Additional Documentation

- **[Skinned Mesh Workflow](skinned-mesh-workflow.md)** - Complete guide for creating and animating skinned meshes
- **[Blender Integration](blender.md)** - Blender MCP server and GLB export documentation