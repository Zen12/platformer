# Asset Creation Guidelines

This document outlines the tools and workflows for creating various types of assets in the project.

## Image Asset Creation

### Tool: GIMP
**All 2D images and sprites should be created using GIMP.**

GIMP is available at `/opt/homebrew/bin/gimp` and should be used for:
- Sprite creation
- UI elements
- Textures
- Icons
- 2D artwork
- Image editing and manipulation

### GIMP Usage
- Use GIMP's batch mode or scripting capabilities when possible
- For programmatic generation, create base images with Python PIL/Pillow, then process with GIMP
- GIMP version: 3.0.6 (as of this writing)

### Image Specifications
- Default size: 512x512 pixels (or as appropriate for the use case)
- Format: PNG with transparency (RGBA) for sprites
- Always create accompanying `.meta` files with:
  - `name`: Asset name
  - `guid`: UUID v4 generated uniquely
  - `extension`: `.png`
  - `type`: `image`

## 3D Asset Creation

### Tool: Blender
**All 3D models, scenes, and renders should be created using Blender.**

Blender is accessible via MCP tools and should be used for:
- 3D model creation
- Scene setup and lighting
- 3D rendering
- Animation
- Material and shader creation for 3D assets

### Blender MCP Tools Available
- `mcp__blender__execute_blender_python`: Execute Python code in Blender
- `mcp__blender__create_blender_object`: Create basic objects (cube, sphere, cylinder, etc.)
- `mcp__blender__save_blender_file`: Save .blend files
- `mcp__blender__get_blender_info`: Get version and system info
- `mcp__blender__list_blender_objects`: List objects in a scene

### Blender Workflow
- Use Python scripting via `bpy` module for programmatic asset creation
- Background mode is used (no GUI)
- Save work as `.blend` files when appropriate

### **CRITICAL: Preview Renders for 3D Models**

**After creating any 3D model, ALWAYS generate a preview render:**

1. **Render a preview image** of the 3D model
2. **Save location**: Same directory as the model file
3. **Naming convention**: `{model_name}_preview.png`
   - Example: If model is `simple_house.blend`, preview is `simple_house_preview.png`
4. **Specifications**:
   - Format: PNG
   - Recommended resolution: 1024x1024 pixels
   - Use proper camera angle and lighting
   - Save in the same directory as the model (e.g., `assets/resources/models/`)

**Example:**
```
Model:   assets/resources/models/simple_house.blend
Preview: assets/resources/models/simple_house_preview.png
```

**After rendering the preview:**
- Call `generate_asset_metadata()` to create the `.meta` file for the preview image
- The preview image gets its own GUID and metadata

### **CRITICAL: Always Update Preview When Model Changes**

**WHENEVER you modify a 3D model, you MUST update its preview:**

1. **Any change to the model** requires a preview update:
   - Adding objects (tree, bushes, roof, etc.)
   - Removing objects
   - Changing materials or colors
   - Adjusting positions or scales
   - Any structural modifications

2. **Update workflow:**
   - Make changes to the model
   - Save the .blend file
   - **IMMEDIATELY render new preview** with same filename
   - Preview automatically replaces the old one
   - Metadata remains unchanged (same GUID)

3. **Never skip the preview update:**
   - The preview must always reflect the current state of the model
   - Outdated previews confuse users and break asset browsing

**Example workflow:**
```
User: "Add a tree to the house"
1. Load simple_house.blend
2. Add tree objects
3. Save simple_house.blend
4. Render simple_house_preview.png  ← REQUIRED
```

This ensures every 3D model has an accurate, up-to-date visual preview that can be displayed in asset browsers and editors.

## Asset Management

### Directory Structure
All assets should be placed in:
```
assets/resources/
├── images/          # 2D images and sprites (GIMP)
├── models/          # 3D models (.blend, .fbx) with _preview.png files
├── materials/       # Material definitions
├── fonts/           # Font files
├── shaders/         # Shader code
├── scenes/          # Scene definitions
├── prefabs/         # Prefab definitions
└── ...
```

**Example of models directory:**
```
assets/resources/models/
├── simple_house.blend
├── simple_house.blend.meta
├── simple_house_preview.png        ← Required preview
├── simple_house_preview.png.meta
├── simple_house.fbx
└── simple_house.fbx.meta
```

### Metadata Files
Every asset file requires a corresponding `.meta` file:
```yaml
name: asset_name
guid: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
extension: .png
type: image
```

### **CRITICAL: Use MCP Tools for Metadata Generation**

**After creating any asset file, ALWAYS use the MCP tool to generate metadata:**

```
generate_asset_metadata()
```

This MCP tool will:
- Scan `assets/resources/` for files without `.meta` files
- Automatically generate GUIDs using UUID v4
- Create properly formatted `.meta` files
- Use `asset_utils.py` for asset type detection

**DO NOT manually create `.meta` files with Python scripts or manual file writing.**

The MCP server provides these asset management tools:
- `generate_asset_metadata()`: Generate .meta files for all assets that don't have them
- `list_assets_without_metadata()`: Check which assets are missing metadata
- `check_asset_references(file_path)`: Check if an asset is referenced elsewhere
- `delete_asset(file_path)`: Safely delete an asset and its metadata

### Asset Utilities Module

The `asset_utils.py` module provides shared functions:
- Asset type mapping (`ext_check()`)
- Extension validation (`get_asset_extensions_with_references()`)
- Asset type definitions (`ASSET_TYPE_MAPPING`, `ASSET_TYPES_WITH_REFERENCES`)

## Tool Selection Quick Reference

| Asset Type | Tool | Notes |
|------------|------|-------|
| Sprites | GIMP | 2D pixel art, characters |
| UI Elements | GIMP | Buttons, icons, panels |
| Textures | GIMP | Surface textures, patterns |
| 3D Models | Blender | Characters, props, environments |
| 3D Renders | Blender | Pre-rendered images from 3D scenes |
| Animations | Blender | 3D animations |

## Installation Notes

- **GIMP**: Installed via Homebrew at `/opt/homebrew/bin/gimp`
- **Blender**: Available via MCP server integration
- **Python PIL/Pillow**: Available in project environment for programmatic image generation

## Best Practices

1. **Use the right tool**: Don't create 3D assets with GIMP or 2D sprites with Blender
2. **ALWAYS use MCP tools for metadata**: After creating any asset file, immediately call `generate_asset_metadata()` - DO NOT manually create `.meta` files
3. **ALWAYS create preview renders for 3D models**: Every 3D model must have a `{model_name}_preview.png` in the same directory
4. **Use transparent backgrounds**: For sprites and UI elements, use RGBA with transparency
5. **Follow naming conventions**: Use lowercase with underscores (e.g., `squirrel_sprite.png`)
6. **Optimize file sizes**: Keep assets reasonably sized for game performance
7. **Version control**: Asset files and their `.meta` files should be committed together
8. **Check references before deleting**: Use `check_asset_references()` before deleting any asset

## Workflow Examples

### Creating a 2D Image Asset

```
1. Create image using GIMP
2. Save to assets/resources/images/my_asset.png
3. Call generate_asset_metadata() MCP tool
4. Verify .meta file was created
5. Commit both files to version control
```

### Creating a 3D Model Asset

```
1. Create 3D model using Blender MCP tools
2. Save to assets/resources/models/my_model.blend (and optionally .fbx)
3. Render preview: assets/resources/models/my_model_preview.png
4. Call generate_asset_metadata() MCP tool
5. Verify .meta files were created for both model and preview
6. Commit all files to version control:
   - my_model.blend + my_model.blend.meta
   - my_model_preview.png + my_model_preview.png.meta
```

**Never skip metadata generation** - metadata is essential for the asset system to function.

**Never skip preview renders for 3D models** - previews are required for asset browsing.