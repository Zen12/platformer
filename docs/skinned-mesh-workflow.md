# Skinned Mesh Workflow Guide

Complete guide for creating, exporting, and animating skinned meshes for the game engine.

## Table of Contents

1. [Overview](#overview)
2. [Blender: Creating Skinned Meshes](#blender-creating-skinned-meshes)
3. [Blender: Exporting to GLB](#blender-exporting-to-glb)
4. [Game Engine Integration](#game-engine-integration)
5. [Troubleshooting](#troubleshooting)

---

## Overview

### What is a Skinned Mesh?

A **skinned mesh** is a 3D model that deforms based on a skeleton (armature) for animation. It consists of:

1. **Mesh** - The 3D geometry (vertices, faces)
2. **Skeleton (Armature)** - Hierarchy of bones
3. **Skin Weights** - How much each vertex is influenced by each bone
4. **Animation** - Keyframe data for bone transformations over time

### Engine Requirements

The game engine uses **Ozz animation system** which requires:

- **18-bone skeleton** structure (simplified for performance)
- **GLB file format** (faster loading than FBX)
- **Specific bone naming** convention (e.g., `Root`, `Hips`, `UpperArm.L`)
- **Matching bone counts** between skeleton and animation

### File Structure

```
assets/resources/models/character/
├── character.blend          # Blender source (editable)
├── character.blend.meta
├── character.glb           # Exported mesh + skeleton (for rendering)
├── character.glb.meta
├── character_preview.png   # Preview render
├── character_preview.png.meta
├── character_texture.png   # Diffuse texture
├── character_texture.png.meta
├── idle.fbx               # Animation source (if using FBX)
├── idle.fbx.meta
├── idle_18bones.glb       # Retargeted animation
└── idle_18bones.glb.meta
```

---

## Blender: Creating Skinned Meshes

### Step 1: Create or Import Mesh

#### Option A: Create from Scratch

1. **Start Blender** with default scene
2. **Delete default cube** (Select → X → Delete)
3. **Add your mesh** (Add → Mesh → ...)
4. **Model your character**

#### Option B: Import Existing Model

```python
# File → Import → FBX/OBJ/GLB
import bpy
bpy.ops.import_scene.fbx(filepath="/path/to/model.fbx")
```

### Step 2: Create Armature (18-Bone Structure)

Our engine uses a **simplified 18-bone skeleton**:

```
Root (parent of Hips)
└── Hips
    ├── Spine
    │   └── Chest
    │       ├── Neck
    │       │   └── Head
    │       ├── UpperArm.L
    │       │   └── Forearm.L
    │       │       └── Hand.L
    │       └── UpperArm.R
    │           └── Forearm.R
    │               └── Hand.R
    ├── Thigh.L
    │   └── Shin.L
    │       └── Foot.L
    └── Thigh.R
        └── Shin.R
            └── Foot.R
```

**Bone naming is CRITICAL** - names must match exactly!

#### Creating the Armature

1. **Add Armature:**
   - Add → Armature
   - Position at origin (0, 0, 0)

2. **Enter Edit Mode:**
   - Select armature
   - Tab → Edit Mode

3. **Create Root Bone:**
   - Select the default bone
   - Rename to "Root"
   - Position head at (0, 0, 0)
   - Position tail at (0, 0, 0.1)

4. **Create Bone Hierarchy:**
   - Add bones using Shift+A → Bone
   - Parent bones correctly (select child, then parent, Ctrl+P → Keep Offset)
   - Name each bone exactly as shown above

**Blender Script to Create 18-Bone Armature:**

```python
import bpy
from mathutils import Vector

# Clear scene
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete()

# Create armature
bpy.ops.object.armature_add(location=(0, 0, 0))
armature_obj = bpy.context.active_object
armature = armature_obj.data

bpy.ops.object.mode_set(mode='EDIT')

# Get default bone and rename to Root
root_bone = armature.edit_bones[0]
root_bone.name = "Root"
root_bone.head = Vector((0, 0, 0))
root_bone.tail = Vector((0, 0, 0.1))

# Define bone structure (name, parent_name, head, tail)
bones_data = [
    ("Hips", "Root", Vector((0, 0, 1)), Vector((0, 0, 1.2))),
    ("Spine", "Hips", Vector((0, 0, 1.2)), Vector((0, 0, 1.5))),
    ("Chest", "Spine", Vector((0, 0, 1.5)), Vector((0, 0, 1.8))),
    ("Neck", "Chest", Vector((0, 0, 1.8)), Vector((0, 0, 2.0))),
    ("Head", "Neck", Vector((0, 0, 2.0)), Vector((0, 0, 2.3))),

    # Left arm
    ("UpperArm.L", "Chest", Vector((0.15, 0, 1.7)), Vector((0.4, 0, 1.7))),
    ("Forearm.L", "UpperArm.L", Vector((0.4, 0, 1.7)), Vector((0.65, 0, 1.7))),
    ("Hand.L", "Forearm.L", Vector((0.65, 0, 1.7)), Vector((0.8, 0, 1.7))),

    # Right arm
    ("UpperArm.R", "Chest", Vector((-0.15, 0, 1.7)), Vector((-0.4, 0, 1.7))),
    ("Forearm.R", "UpperArm.R", Vector((-0.4, 0, 1.7)), Vector((-0.65, 0, 1.7))),
    ("Hand.R", "Forearm.R", Vector((-0.65, 0, 1.7)), Vector((-0.8, 0, 1.7))),

    # Left leg
    ("Thigh.L", "Hips", Vector((0.1, 0, 1.0)), Vector((0.1, 0, 0.5))),
    ("Shin.L", "Thigh.L", Vector((0.1, 0, 0.5)), Vector((0.1, 0, 0.0))),
    ("Foot.L", "Shin.L", Vector((0.1, 0, 0.0)), Vector((0.1, 0.15, 0.0))),

    # Right leg
    ("Thigh.R", "Hips", Vector((-0.1, 0, 1.0)), Vector((-0.1, 0, 0.5))),
    ("Shin.R", "Thigh.R", Vector((-0.1, 0, 0.5)), Vector((-0.1, 0, 0.0))),
    ("Foot.R", "Shin.R", Vector((-0.1, 0, 0.0)), Vector((-0.1, 0.15, 0.0))),
]

# Create bones
for bone_name, parent_name, head, tail in bones_data:
    bone = armature.edit_bones.new(bone_name)
    bone.head = head
    bone.tail = tail

    # Set parent
    if parent_name in armature.edit_bones:
        bone.parent = armature.edit_bones[parent_name]

bpy.ops.object.mode_set(mode='OBJECT')
```

### Step 3: Skin the Mesh (Weight Painting)

1. **Select Mesh, then Armature** (in that order)
2. **Parent with Automatic Weights:**
   - Ctrl+P → With Automatic Weights
   - Blender calculates initial bone weights

3. **Refine Weights (Optional):**
   - Select mesh
   - Tab → Weight Paint mode
   - Select bones in armature (shows weight influence)
   - Paint to adjust influence (Red = 100%, Blue = 0%)

**Tips for Good Weights:**
- Joints should have smooth falloff between bones
- Each vertex should be influenced by 1-4 bones maximum
- Test deformation by posing armature in Pose Mode

### Step 4: Add Texture (Optional)

1. **UV Unwrap:**
   - Select mesh → Tab (Edit Mode)
   - U → Smart UV Project (or other method)

2. **Create Material:**
   - Shading workspace
   - Add → Image Texture node
   - Load your texture image

3. **Bake Texture:**
   - If needed, bake procedural materials to image

---

## Blender: Exporting to GLB

### Export Settings

**File → Export → glTF 2.0 (.glb/.gltf)**

**CRITICAL Settings:**

```
Format: GLB Binary (.glb)
Include:
  ✓ Selected Objects (or uncheck to export all)
  ✓ Custom Properties
Transform:
  +Y Up (default)
Geometry:
  ✓ Apply Modifiers
  ✓ UVs
  ✓ Normals
  ✓ Tangents
  ✓ Vertex Colors
Material:
  Materials: Export
  Images: Automatic (or None if using separate texture)
Animation:
  ✓ Animation (if exporting with animation)
  ✓ Limit to Playback Range
  ✓ Sampling Rate (default)
  ✓ Group by NLA Track
  ✓ Export Deformation Bones Only
  ✗ Shape Keys (disable unless using)
  ✗ Shape Key Normals
  ✗ Skinning (✓ enable!)
Skinning:
  ✓ Include All Bone Influences
  ✓ Export all bone influences (don't limit to 4)
```

**Via Python (Recommended for Consistency):**

```python
import bpy

# Export mesh + skeleton (no animation)
bpy.ops.export_scene.gltf(
    filepath="/path/to/character.glb",
    export_format='GLB',
    use_selection=False,  # Export all, or True for selected only
    export_animations=False,  # No animation, just mesh + skeleton
    export_def_bones=True,  # Include deformation bones
    export_materials='EXPORT',  # Export materials
    export_textures=True,  # Export embedded textures (or False for external)
    export_apply=True  # Apply modifiers
)
```

### What Gets Exported

**character.glb contains:**
- Mesh geometry (vertices, indices)
- Vertex attributes (positions, normals, UVs, bone weights, bone indices)
- Skeleton (18 bones with hierarchy)
- Bind pose (rest position)
- Material data (optional)
- Textures (optional, can be external)

**character.glb does NOT contain:**
- Animation keyframes (export separately)

---

## Game Engine Integration

### Step 1: Generate Metadata

After exporting GLB files, generate `.meta` files:

```bash
# Generate metadata for all new files
python3 mcp_tools/import.py assets/resources/models/character/
```

Or use MCP server:
```python
generate_asset_metadata()
```

This creates:
- `character.glb.meta` (mesh + skeleton GUID)
- `idle_18bones.glb.meta` (animation GUID)

### Step 2: Create Material

Create a material file for the character:

```yaml
# assets/resources/materials/character.mat
name: character_material
is_culling: True
blend_mode: 0
shader_opengl:
  vertex: <vertex_shader_guid>
  fragment: <fragment_shader_guid>
shader_gles:
  vertex: <vertex_shader_guid_gles>
  fragment: <fragment_shader_guid_gles>
image: <texture_guid>  # character_texture.png GUID
```

### Step 3: Create Scene Entity

Add character to scene:

```yaml
# assets/resources/scenes/game.scene
entities:
  - tag: "character"
    components:
      - type: transform
        position: [0.0, 0.0, 0.0]
        rotation: [0.0, 0.0, 0.0]
        scale: [1.0, 1.0, 1.0]

      - type: skinned_mesh_renderer
        material: <character_material_guid>
        mesh: <character.glb_guid>

      - type: ozz_skeleton
        skeleton_guid: <character.glb_guid>  # Same as mesh

      - type: ozz_animation
        animation_guid: <idle_18bones.glb_guid>
        playback_speed: 1.0
        loop: true
        playing: true
```

**Key Points:**
- `mesh` and `skeleton_guid` use the SAME GUID (character.glb)
- `animation_guid` uses the retargeted animation GUID
- GUIDs are from `.meta` files

### Step 4: Build and Test

```bash
./run_debug.sh
```

**Expected Result:**
- Character mesh loads
- Skeleton loads (19 joints including Root)
- Animation plays and loops
- Mesh deforms correctly with bone transforms

---

## Troubleshooting

### Issue: "No bones found in mesh"

**Cause:** GLB export didn't include skeleton data

**Fix:**
- Re-export with "Export Deformation Bones Only" enabled
- Check "Skinning" section in export settings
- Ensure mesh is parented to armature (Ctrl+P → With Automatic Weights)

### Issue: "Skeleton bone count != Animation bone count"

**Cause:** Skeleton and animation have different bone counts

**Fix:**
- Verify character.glb has 18 bones
- Verify animation was retargeted to 18 bones
- Check animation GUID is correct (should be idle_18bones.glb, not Idle.fbx)

### Issue: "Mesh is displaced vertically/horizontally"

**Cause:** Rest pose mismatch between mesh and animation

**Fix:**
- Ensure Root bone is at origin (0, 0, 0) in both files
- Re-run retargeting script (it auto-aligns Root bone)
- Check armature object location is (0, 0, 0) and scale is (1, 1, 1)

### Issue: "Animation not playing"

**Possible Causes & Fixes:**

1. **Animation GUID wrong:**
   - Check `ozz_animation` component uses correct GUID
   - Verify GUID matches `idle_18bones.glb.meta`

2. **Animation not looping:**
   - Set `loop: true` in scene file
   - Check animation duration > 0

3. **Bone names don't match:**
   - Verify bone names exactly match (case-sensitive)
   - Run retargeting script to ensure correct naming

### Issue: "Mesh deforms incorrectly"

**Cause:** Bone weights or hierarchy problems

**Fix:**
- Check weight painting in Blender (Weight Paint mode)
- Verify bone hierarchy is correct (Hips → Spine → Chest → etc.)
- Ensure all bones have proper parent relationships
- Re-apply automatic weights and test

### Issue: "Animation plays but mesh doesn't move"

**Cause:** Bone offset matrices not loading

**Fix:**
- Check mesh has bone weight data (vertex attributes)
- Verify mesh was parented with "Automatic Weights"
- Re-export mesh with skinning enabled

---

## Best Practices

### Modeling
- Keep mesh relatively low-poly (< 5000 vertices for mobile)
- Use quads where possible, triangulate before export
- Ensure all faces have normals pointing outward

### Rigging
- Use exactly 18 bones (no more, no less)
- Follow exact bone naming convention
- Keep bone hierarchy simple and clean
- Root bone must be at origin

### Skinning
- Limit bone influences to 4 per vertex (engine limitation)
- Smooth weights at joints for natural deformation
- Test deformation by posing bones before export

### Textures
- Use power-of-2 dimensions (512x512, 1024x1024, etc.)
- PNG format with transparency for characters
- Keep under 2MB for mobile
- External textures preferred over embedded (faster loading)

### Animations
- Create animations in Blender or import from external sources
- Always retarget to 18 bones before use
- Use 30 fps or match game's target fps
- Trim unnecessary frames to reduce file size

### File Organization
```
models/character/
├── character.blend         # Source (never delete)
├── character.glb          # Engine format (mesh + skeleton)
├── character_preview.png  # For asset browser
├── character_texture.png  # Diffuse map
├── idle.fbx              # Animation source (optional)
├── idle_18bones.glb      # Retargeted animation (keep)
├── walk_18bones.glb      # More animations...
└── *.meta                # Metadata for all files
```

---

## Quick Reference

### Blender Shortcuts
- `Tab` - Toggle Edit/Object mode
- `Shift+A` - Add (mesh, armature, bone)
- `Ctrl+P` - Parent (mesh to armature)
- `Alt+P` - Clear parent
- `Ctrl+A` - Apply transforms
- `G/R/S` - Move/Rotate/Scale

### Bone Naming Conventions
```
Root              # Parent of Hips
Hips              # Pelvis
Spine             # Lower spine
Chest             # Upper spine/chest
Neck              # Neck
Head              # Head
UpperArm.L/R      # Upper arm
Forearm.L/R       # Lower arm
Hand.L/R          # Hand
Thigh.L/R         # Upper leg
Shin.L/R          # Lower leg
Foot.L/R          # Foot
```

### Export Checklist
- [ ] Mesh has 18-bone armature
- [ ] Bones are named correctly
- [ ] Mesh is parented with automatic weights
- [ ] Armature is at origin (0, 0, 0)
- [ ] Transforms applied (Ctrl+A)
- [ ] Export format: GLB
- [ ] Skinning enabled in export
- [ ] Generated .meta file

### Animation Checklist
- [ ] Animation created or imported
- [ ] Retargeted to 18 bones
- [ ] Exported to GLB format
- [ ] Generated .meta file
- [ ] GUID added to scene file
- [ ] Animation plays and loops

---

## See Also

- [Blender Documentation](blender.md) - Blender MCP server and GLB export
- [MCP Tools Documentation](mcp.md) - Asset management tools
- [Asset Creation Guidelines](../README.md) - General asset rules
- [Coding Style - Component Factory](../.claude/rules/coding-style.md) - Adding new components

---

## Credits

- **Ozz Animation Library** - [GitHub](https://github.com/guillaumeblanc/ozz-animation)
- **Blender** - [blender.org](https://www.blender.org)
