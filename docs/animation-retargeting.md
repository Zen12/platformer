# Animation Retargeting Guide

## Mixamo Animation Retargeting

### Overview

Retarget Mixamo animations (65 bones) to a simplified 18-bone skeleton compatible with the game engine's Ozz animation system. This tool is available through the MCP server or as a standalone script.

### Bone Structure

**Mixamo (65 bones)** → **Game Engine (18 bones)**

The retargeting process keeps these 17 bones from Mixamo and adds a Root bone:

| Mixamo Bone Name       | Game Engine Bone Name |
|------------------------|----------------------|
| mixamorig:Hips         | Hips                 |
| mixamorig:Spine        | Spine                |
| mixamorig:Spine1       | Chest                |
| mixamorig:Neck         | Neck                 |
| mixamorig:Head         | Head                 |
| mixamorig:LeftArm      | UpperArm.L           |
| mixamorig:LeftForeArm  | Forearm.L            |
| mixamorig:LeftHand     | Hand.L               |
| mixamorig:RightArm     | UpperArm.R           |
| mixamorig:RightForeArm | Forearm.R            |
| mixamorig:RightHand    | Hand.R               |
| mixamorig:LeftUpLeg    | Thigh.L              |
| mixamorig:LeftLeg      | Shin.L               |
| mixamorig:LeftFoot     | Foot.L               |
| mixamorig:RightUpLeg   | Thigh.R              |
| mixamorig:RightLeg     | Shin.R               |
| mixamorig:RightFoot    | Foot.R               |
| (new)                  | Root                 |

**Deleted bones:** Fingers, toes, shoulders, extra spine bones (48 total)

### Coordinate System Conversion

The retargeting script automatically converts from **Mixamo/Blender Z-up** to **Game Engine Y-up** orientation:

**Default Rotation Applied:**
1. **-90° around X-axis** - Converts Z-up to Y-up
2. **+90° around Z-axis** - Fixes facing direction (character faces forward)

This ensures animations work correctly in the game engine without requiring rotation adjustments in scene files.

**Result:** Exported GLB files are ready to use with default scene settings (rotation: [0, 0, 0])

### Usage

There are three ways to retarget animations:

#### Method 1: MCP Server (Recommended)

Use the Blender MCP server tool directly:

```python
# In Claude Code, just call:
retarget_mixamo_animation(
    input_fbx="assets/resources/models/shooter/Idle.fbx",
    output_glb="assets/resources/models/shooter/idle_18bones.glb"
)
```

**Advantages:**
- Automatic path resolution
- Error handling and validation
- Integrated with asset workflow
- No manual Blender command needed

#### Method 2: Command Line (Standalone Script)

```bash
blender --background --python mcp_tools/retarget_mixamo_animation.py -- \
    assets/resources/models/shooter/Idle.fbx \
    assets/resources/models/shooter/idle_18bones.glb
```

**Use when:**
- Running outside of Claude Code
- Batch processing multiple animations
- Scripting or automation

#### Method 3: Python Module

```python
from mcp_tools.retarget_mixamo_animation import retarget_mixamo_animation

success = retarget_mixamo_animation(
    "/path/to/Idle.fbx",
    "/path/to/idle_18bones.glb"
)
```

**Use when:**
- Building custom tools
- Integrating into other Python scripts

### Complete Workflow

1. **Download Mixamo animation** (FBX format with skin)

2. **Retarget animation** using one of the methods above:
   ```python
   # MCP server (recommended)
   retarget_mixamo_animation(
       "assets/resources/models/shooter/Idle.fbx",
       "assets/resources/models/shooter/idle_18bones.glb"
   )
   ```

3. **Generate metadata** for the new GLB file:
   ```python
   # MCP server
   generate_asset_metadata()

   # Or command line
   python3 mcp_tools/server/asset_server.py
   ```

4. **Update scene file** to use the new animation:
   - Find the GUID in `assets/resources/models/shooter/idle_18bones.glb.meta`
   - Update `test.scene`:
   ```yaml
   - type: ozz_animation
     animation_guid: <guid-from-meta-file>
     playback_speed: 1.0
     loop: true
     playing: true
   ```

5. **Build and test**:
   ```bash
   ./run_debug.sh
   ```

### How It Works

1. **Import Mixamo FBX**: Loads the 65-bone Mixamo skeleton with animation
2. **Delete 48 bones**: Removes fingers, toes, shoulders, extra spine bones
3. **Rename 17 bones**: Maps Mixamo names to game engine names
4. **Add Root bone**: Creates a Root bone at origin as parent of Hips
5. **Align rest pose**: Offsets armature so Root bone is at world origin (0,0,0)
6. **Apply transform**: Bakes the offset into skeleton data
7. **Export GLB**: Saves with 18 bones and animation data

### Important Notes

- **Rest pose alignment**: The script automatically adjusts the armature position so the Root bone is at the origin. This ensures animations align with your mesh's rest pose.
- **Bone count matching**: The output will have exactly 18 bones, matching the game engine's expected structure.
- **Animation preservation**: All keyframes are preserved and retargeted to the simplified skeleton.
- **File format**: GLB is preferred over FBX for 5-50x faster loading in the game engine.

### Troubleshooting

**Animation not playing:**
- Verify bone count: Should be 18 bones in the output GLB
- Check metadata: Animation GUID must match the `.meta` file
- Confirm scene setup: `ozz_skeleton` should use mesh GUID, `ozz_animation` should use animation GUID

**Vertical displacement:**
- This should not happen with this script (Root bone is aligned at origin)
- If it does occur, check that the mesh file also has Root bone at origin

**Bone names don't match:**
- The script uses exact naming from the bone mapping table above
- Verify your mesh uses the same bone names (UpperArm.L, Thigh.R, etc.)

### Requirements

- **Blender**: 3.0 or higher (tested with 3.6)
- **Python**: Blender's built-in Python (no external dependencies)
- **Input**: Mixamo FBX animation file
- **Output**: GLB file with retargeted animation

### Advanced: Modifying Bone Mappings

To customize which bones to keep, edit the `bones_to_keep` dictionary in:
- `mcp_tools/retarget_mixamo_animation.py` (standalone script)
- `mcp_tools/server/blender_server.py` (MCP server function)

```python
bones_to_keep = {
    "mixamorig:Hips": "Hips",
    # Add or remove bones here
}
```

Make sure the total bone count (including Root) matches your mesh's bone count.

### Files

- **Script**: `mcp_tools/retarget_mixamo_animation.py` - Standalone Blender script
- **MCP Server**: `mcp_tools/server/blender_server.py` - MCP tool integration
- **Documentation**: `docs/animation-retargeting.md` - This file

### See Also

- [Ozz Animation System](ozz-animation.md) - Animation system architecture
- [Asset Creation Guidelines](../README.md) - General asset workflow
- [MCP Tools Documentation](../mcp_tools/README.md) - MCP server usage
