#!/usr/bin/env python3
"""
Mixamo Animation Retargeting for Game Engine

Retargets Mixamo animations (65 bones) to simplified 18-bone skeleton compatible
with the game engine's Ozz animation system.

USAGE - Command Line (Direct):
    blender --background --python mcp_tools/retarget_mixamo_animation.py -- <input_fbx> <output_glb>

    Example:
    blender --background --python mcp_tools/retarget_mixamo_animation.py -- \\
        assets/resources/models/shooter/Idle.fbx \\
        assets/resources/models/shooter/idle_18bones.glb

USAGE - From MCP Server (Recommended):
    Use the Blender MCP server tool 'retarget_mixamo_animation':

    retarget_mixamo_animation(
        input_fbx="assets/resources/models/shooter/Idle.fbx",
        output_glb="assets/resources/models/shooter/idle_18bones.glb"
    )

USAGE - As Python Module:
    from mcp_tools.retarget_mixamo_animation import retarget_mixamo_animation

    success = retarget_mixamo_animation(
        "/path/to/Idle.fbx",
        "/path/to/idle_18bones.glb"
    )

COORDINATE SYSTEM CONVERSION:
    Automatically converts from Mixamo/Blender Z-up to Game Engine Y-up:
    - Step 1: Rotate -90° around X-axis (Z-up → Y-up)
    - Step 2: Rotate +90° around Z-axis (fix facing direction)

    Result: Exported GLB works with default scene settings (rotation: [0, 0, 0])

WORKFLOW:
    1. Download animation from Mixamo (FBX format, "With Skin")
    2. Run this script to retarget to 18 bones (automatic coordinate conversion)
    3. Generate metadata: python3 mcp_tools/import.py
    4. Update scene to use new animation GUID
    5. Build and test: ./run_debug.sh

BONE MAPPING:
    Mixamo (65 bones) → Game Engine (18 bones)

    Kept bones (17 from Mixamo + 1 Root):
    - mixamorig:Hips        → Hips
    - mixamorig:Spine       → Spine
    - mixamorig:Spine1      → Chest
    - mixamorig:Neck        → Neck
    - mixamorig:Head        → Head
    - mixamorig:LeftArm     → UpperArm.L
    - mixamorig:LeftForeArm → Forearm.L
    - mixamorig:LeftHand    → Hand.L
    - mixamorig:RightArm    → UpperArm.R
    - mixamorig:RightForeArm→ Forearm.R
    - mixamorig:RightHand   → Hand.R
    - mixamorig:LeftUpLeg   → Thigh.L
    - mixamorig:LeftLeg     → Shin.L
    - mixamorig:LeftFoot    → Foot.L
    - mixamorig:RightUpLeg  → Thigh.R
    - mixamorig:RightLeg    → Shin.R
    - mixamorig:RightFoot   → Foot.R
    - (new)                 → Root (parent of Hips)

    Deleted bones (48): Fingers, toes, shoulders, extra spine bones

TECHNICAL DETAILS:
    - Deletes 48 unnecessary bones for simplified skeleton
    - Renames bones to match game engine naming convention
    - Adds Root bone at origin as parent of Hips
    - Offsets armature to align Root at world origin (0,0,0)
    - Applies transform to bake offset into skeleton data
    - Exports to GLB format (5-50x faster loading than FBX)
    - Preserves all animation keyframes during retargeting

Requirements:
    - Blender 3.0+
    - Input: Mixamo FBX animation file (65 bones)
    - Output: GLB file with retargeted animation (18 bones)

See Also:
    - docs/animation-retargeting.md - Complete documentation
    - mcp_tools/server/blender_server.py - MCP server integration
"""

import bpy
import sys
from mathutils import Vector

def retarget_mixamo_animation(input_fbx_path, output_glb_path):
    """
    Retarget Mixamo animation to 18-bone skeleton.

    Args:
        input_fbx_path: Path to Mixamo FBX file (65 bones)
        output_glb_path: Path to save retargeted GLB file (18 bones)
    """
    # Clear default scene
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete()

    # Import Mixamo FBX
    bpy.ops.import_scene.fbx(filepath=input_fbx_path)

    # Find armature
    armature_obj = None
    for obj in bpy.data.objects:
        if obj.type == 'ARMATURE':
            armature_obj = obj
            break

    if not armature_obj:
        return False

    armature = armature_obj.data

    # Define bone mapping (17 Mixamo bones to keep)
    # These map to the 18-bone structure (+ Root) used by the game engine
    bones_to_keep = {
        "mixamorig:Hips": "Hips",
        "mixamorig:Spine": "Spine",
        "mixamorig:Spine1": "Chest",     # Mixamo Spine1 -> Chest
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
    }

    # Switch to edit mode
    bpy.context.view_layer.objects.active = armature_obj
    bpy.ops.object.mode_set(mode='EDIT')

    # Get Hips position before modifications
    hips_bone = armature.edit_bones.get("mixamorig:Hips")
    if hips_bone:
        hips_y_pos = hips_bone.head.y
    else:
        hips_y_pos = 0

    # Delete bones not in the mapping
    bones_to_delete = [bone.name for bone in armature.edit_bones
                      if bone.name not in bones_to_keep]
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
        print(f"Hips position after reparenting: {hips_bone.head}")
    else:
        print("ERROR: Could not find Hips bone for reparenting")
        return False

    # Switch to object mode
    bpy.ops.object.mode_set(mode='OBJECT')

    # CRITICAL: Offset armature object to align Root at world origin
    # Mixamo's Hips bone is usually at Y≈104 in Blender units
    # We need to move the armature down so Root aligns with the mesh's rest pose
    print(f"Original armature location: {armature_obj.location}")
    armature_obj.location.y -= hips_y_pos
    print(f"New armature location: {armature_obj.location}")

    # Apply transform to bake offset into skeleton
    bpy.ops.object.select_all(action='DESELECT')
    armature_obj.select_set(True)
    bpy.context.view_layer.objects.active = armature_obj
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    print("Applied transform to armature")

    # CRITICAL: Rotate all objects (armature + mesh) to convert Z-up to Y-up
    # Select all objects and rotate together to maintain relative positions
    import math

    # Make sure we're in object mode
    bpy.ops.object.mode_set(mode='OBJECT')

    # Select all objects
    bpy.ops.object.select_all(action='SELECT')

    # Step 1: Rotate -90° around X-axis (Z-up -> Y-up)
    bpy.ops.transform.rotate(value=math.radians(-90), orient_axis='X', orient_type='GLOBAL')
    print("Rotated all objects -90° around X-axis (Z-up -> Y-up)")

    # Step 2: Rotate 90° around Z-axis (fix facing direction)
    bpy.ops.transform.rotate(value=math.radians(90), orient_axis='Z', orient_type='GLOBAL')
    print("Rotated all objects 90° around Z-axis (fix facing direction)")

    # Apply transform to bake rotation into mesh and bones
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    print("Applied rotation to all objects")

    # Verify final bone count
    bone_count = len(armature.bones)
    print(f"=== Final bone count: {bone_count} ===")

    # Check for animations
    if bpy.data.actions:
        for action in bpy.data.actions:
            print(f"Animation: {action.name}, frames: {int(action.frame_range[0])} - {int(action.frame_range[1])}")
    else:
        print("WARNING: No animations found in file")

    # Export to GLB
    print("=== EXPORTING TO GLB ===")
    bpy.ops.export_scene.gltf(
        filepath=output_glb_path,
        export_format='GLB',
        use_selection=False,
        export_animations=True,
        export_frame_range=True,
        export_force_sampling=True,
        export_def_bones=True,
        export_rest_position_armature=False
        # Note: Not using export_yup since we manually rotated the scene
    )
    print("=== EXPORT COMPLETE ===")

    # Check file size
    import os
    if os.path.exists(output_glb_path):
        size_kb = os.path.getsize(output_glb_path) / 1024
        print(f"File size: {size_kb:.1f} KB")
        return True
    else:
        print("ERROR: Export failed, file not created")
        return False


if __name__ == "__main__":
    # Parse command line arguments
    # Blender passes args after '--' to the script
    argv = sys.argv
    if "--" in argv:
        argv = argv[argv.index("--") + 1:]
    else:
        print("Usage: blender --background --python retarget_mixamo_animation.py -- <input_fbx> <output_glb>")
        sys.exit(1)

    if len(argv) < 2:
        print("ERROR: Missing arguments")
        print("Usage: blender --background --python retarget_mixamo_animation.py -- <input_fbx> <output_glb>")
        sys.exit(1)

    input_fbx = argv[0]
    output_glb = argv[1]

    print(f"Input: {input_fbx}")
    print(f"Output: {output_glb}")

    success = retarget_mixamo_animation(input_fbx, output_glb)
    sys.exit(0 if success else 1)
