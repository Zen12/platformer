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

## Usage Examples

### In Claude Code

```
- "Create a new project called 'My Game' with 1920x1080 resolution"
- "Create a new FSM called 'menu_fsm'"
- "Create a new shader called 'custom_shader'"
- "Generate metadata for all assets"
- "List assets without metadata"
- "Find all references to awesomeface.png"
- "Delete sprite2.mat"
```

### Command Line

```bash
# Create a new project
python3 mcp/create.py project --name "My Game" --resolution 1920 1080 --fps 60

# Create a new FSM
python3 mcp/create.py fsm --name menu_fsm

# Create a new shader (GL + GLES with unique GUIDs)
python3 mcp/create.py shader --name custom_shader

# Find all references to an asset
python3 mcp/find.py assets/resources/images/player.png

# Find references by GUID
python3 mcp/find.py --guid f22681f4-45d1-4542-a838-99f854890c18

# Import/create metadata for new assets
python3 mcp/import.py                          # All assets
python3 mcp/import.py assets/resources/images  # Specific folder
python3 mcp/import.py assets/resources/test.png # Specific file

# Delete assets with reference checking
python3 mcp/delete.py assets/resources/test.png
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

- `asset_server.py` - Main MCP server implementation
- `asset_utils.py` - Shared utilities for asset management
- `create.py` - Project and asset creation utilities
- `find.py` - GUID reference finder
- `import.py` - Asset metadata generator
- `delete.py` - Safe asset deletion with reference checking