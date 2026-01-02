#!/usr/bin/env python3
"""
Blender to GLB Export Utility

Exports Blender files (.blend) to GLB format optimized for the game engine.
Supports various export options including compression, animation export, and more.

USAGE - Command Line (Direct):
    blender --background --python mcp_tools/export_to_glb.py -- <input_blend> <output_glb> [options]

    Examples:
    # Basic export
    blender --background --python mcp_tools/export_to_glb.py -- \\
        assets/resources/models/my_model.blend \\
        assets/resources/models/my_model.glb

    # Export with Draco compression
    blender --background --python mcp_tools/export_to_glb.py -- \\
        assets/resources/models/large_model.blend \\
        assets/resources/models/large_model.glb \\
        --compress

    # Export with animations
    blender --background --python mcp_tools/export_to_glb.py -- \\
        assets/resources/models/character.blend \\
        assets/resources/models/character.glb \\
        --animations

USAGE - As Python Module:
    from mcp_tools.export_to_glb import export_to_glb

    # Basic export
    success = export_to_glb(
        "/path/to/model.blend",
        "/path/to/model.glb"
    )

    # With compression
    success = export_to_glb(
        "/path/to/model.blend",
        "/path/to/model.glb",
        use_compression=True,
        compression_level=6
    )

    # With animations
    success = export_to_glb(
        "/path/to/model.blend",
        "/path/to/model.glb",
        export_animations=True
    )

    # With custom scale (e.g., scale up by 10x)
    success = export_to_glb(
        "/path/to/model.blend",
        "/path/to/model.glb",
        scale_factor=10.0
    )

GLB FORMAT BENEFITS:
    - 5-50x faster loading compared to FBX
    - Smaller file sizes with efficient binary encoding
    - Optimized for real-time rendering (GPU-ready)
    - Industry standard format (Unity, Unreal, Godot, Three.js)

WORKFLOW:
    1. Create/edit model in Blender (.blend file)
    2. Run this script to export to GLB
    3. Generate metadata: python3 mcp_tools/import.py
    4. Use in game engine

EXPORT OPTIONS:
    --compress              Enable Draco mesh compression (for large models)
    --compression-level N   Compression level 0-10 (default: 6, higher = smaller)
    --animations            Export animations with the model
    --selection-only        Export only selected objects
    --apply-modifiers       Apply modifiers before export (default: True)
    --scale N               Scale factor for mesh and bones (default: 1.0)
    --no-yup                Preserve Z-up orientation (default: convert to Y-up)

NOTE.TXT FILES:
    Before exporting, check if a note.txt file exists in the model's directory.
    This file may contain specific export settings required for that model/animation.

    Example: assets/resources/models/zombie/original/note.txt
    Contains export settings like --no-yup for Z-up animations.

COORDINATE SYSTEM:
    The game engine mesh loader automatically converts from Blender's coordinate
    system to the engine's coordinate system. You can model naturally in Blender
    and export with default settings.

Requirements:
    - Blender 3.0+
    - Input: Blender file (.blend)
    - Output: GLB file (binary glTF)

See Also:
    - docs/blender.md - Blender integration documentation
"""

import bpy
import sys
import os


def remove_root_motion():
    """
    Remove root bone translation to make animation in-place.
    Keeps rotation and scale, but zeros out X/Y/Z location on root bone.
    """
    # Find the armature
    armature = None
    for obj in bpy.data.objects:
        if obj.type == 'ARMATURE':
            armature = obj
            break

    if not armature:
        print("No armature found, skipping root motion removal")
        return

    # Print bone hierarchy to help identify root
    print(f"Armature bones ({len(armature.data.bones)} total):")
    for i, bone in enumerate(armature.data.bones):
        parent_name = bone.parent.name if bone.parent else "None"
        print(f"  [{i}] {bone.name} (parent: {parent_name})")
        if i >= 5:  # Just show first few
            print(f"  ... and {len(armature.data.bones) - 5} more")
            break

    # Create animation data if it doesn't exist
    if not armature.animation_data:
        print("No animation data on armature")
        return

    # Get the current action
    action = armature.animation_data.action
    if not action:
        # Try to find an action in bpy.data.actions and assign it
        if bpy.data.actions:
            action = list(bpy.data.actions)[0]
            armature.animation_data.action = action
            print(f"Assigned action: {action.name}")
        else:
            print("No actions found")
            return

    print(f"Processing action: {action.name}")

    # Try to access fcurves through animation_data (Blender 5.0 compatibility)
    try:
        # Access fcurves from armature's animation_data, not from action directly
        if hasattr(armature.animation_data, 'action') and armature.animation_data.action:
            # Try to get drivers or use bpy.data
            fcurves_list = []
            # Check if action has channels
            if hasattr(action, 'channels'):
                print(f"Action has {len(action.channels)} channels")
            # Try accessing through bpy.data
            for act in bpy.data.actions:
                if act.name == action.name:
                    # Try different access methods
                    if hasattr(act, 'fcurves'):
                        fcurves_list = list(act.fcurves)
                    elif hasattr(act, 'curves'):
                        fcurves_list = list(act.curves)
                    break

        if not fcurves_list:
            print("Could not access fcurves - Blender API limitation")
            print("Skipping root motion removal - handle in game engine")
            return

        print(f"Found {len(fcurves_list)} fcurves")

        # Find root bone (first bone with no parent)
        root_bone_name = None
        for bone in armature.data.bones:
            if bone.parent is None:
                root_bone_name = bone.name
                print(f"Root bone identified: {root_bone_name}")
                break

        if not root_bone_name:
            print("Could not identify root bone")
            return

        # Zero out location fcurves for root bone
        removed_curves = 0
        for fcurve in fcurves_list:
            if "location" in fcurve.data_path and root_bone_name in fcurve.data_path:
                for keyframe in fcurve.keyframe_points:
                    keyframe.co[1] = 0.0
                    keyframe.handle_left[1] = 0.0
                    keyframe.handle_right[1] = 0.0
                removed_curves += 1
                print(f"  Zeroed location curve (axis {fcurve.array_index})")

        if removed_curves > 0:
            print(f"Removed root motion: {removed_curves} location curves zeroed")
        else:
            print("No root motion found to remove")

    except Exception as e:
        print(f"Error accessing fcurves: {e}")
        import traceback
        traceback.print_exc()


def export_to_glb(
    input_blend_path,
    output_glb_path,
    use_compression=False,
    compression_level=6,
    export_animations=False,
    export_selection_only=False,
    apply_modifiers=True,
    scale_factor=1.0,
    export_yup=True,
    apply_rotation_fix=True,
    remove_root_motion_flag=False
):
    """
    Export Blender file to GLB format.

    Args:
        input_blend_path: Path to Blender file (.blend) or FBX file (.fbx)
        output_glb_path: Path to save GLB file
        use_compression: Enable Draco mesh compression (for large models)
        compression_level: Compression level 0-10 (higher = smaller file)
        export_animations: Include animations in export
        export_selection_only: Export only selected objects
        apply_modifiers: Apply modifiers before export
        scale_factor: Scale factor to apply to mesh and bones (default: 1.0)
        export_yup: Convert to Y-up orientation (default: True, set False to preserve Z-up)
        apply_rotation_fix: Apply Y-forward to Z-forward rotation (default: True, set False for animations)
        remove_root_motion_flag: Remove root bone translation for in-place animations (default: False)

    Returns:
        bool: True if export succeeded, False otherwise
    """
    # Clear default scene
    bpy.ops.wm.read_homefile(use_empty=True)

    # Load the file (detect format)
    print(f"Loading: {input_blend_path}")
    file_ext = os.path.splitext(input_blend_path)[1].lower()

    if file_ext == '.fbx':
        # Import FBX file
        bpy.ops.import_scene.fbx(filepath=input_blend_path)
        print(f"Imported FBX: {input_blend_path}")
    elif file_ext == '.blend':
        # Open blend file
        bpy.ops.wm.open_mainfile(filepath=input_blend_path)
        print(f"Opened blend file: {input_blend_path}")
    else:
        print(f"Error: Unsupported file format: {file_ext}")
        print(f"Supported formats: .blend, .fbx")
        return False

    print(f"Loaded {len(bpy.data.objects)} objects")

    # List objects for debugging
    for obj in bpy.data.objects:
        print(f"  - {obj.name} ({obj.type})")

    # Apply scale factor if not 1.0
    if scale_factor != 1.0:
        print(f"Applying scale factor: {scale_factor}")
        bpy.ops.object.select_all(action='SELECT')
        bpy.ops.transform.resize(value=(scale_factor, scale_factor, scale_factor))

        # Apply scale to bake it into the mesh and armature
        for obj in bpy.data.objects:
            if obj.type in ['MESH', 'ARMATURE']:
                bpy.context.view_layer.objects.active = obj
                bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
                print(f"  Applied scale to: {obj.name}")

        print("Scale applied and baked into objects")

    # Fix Y-forward to Z-forward for models that use Y as forward axis
    # Rotate -90 degrees around X axis to convert Y-forward to Z-forward
    # Skip for animations to preserve bone-local transforms
    if not export_yup and apply_rotation_fix:  # Only for Z-up models (like zombie static mesh)
        print("Converting Y-forward to Z-forward (rotating -90° around X axis)")
        bpy.ops.object.select_all(action='SELECT')
        import math
        bpy.ops.transform.rotate(value=math.radians(-90), orient_axis='X', orient_type='GLOBAL')

        # Apply rotation to bake it
        for obj in bpy.data.objects:
            if obj.type in ['MESH', 'ARMATURE']:
                bpy.context.view_layer.objects.active = obj
                bpy.ops.object.transform_apply(location=False, rotation=True, scale=False)
                print(f"  Applied rotation to: {obj.name}")

        print("Y-forward to Z-forward conversion applied")
    elif not export_yup and not apply_rotation_fix:
        print("Skipping Y-forward to Z-forward conversion (apply_rotation_fix=False)")

    # Build export options
    export_options = {
        'filepath': output_glb_path,
        'export_format': 'GLB',  # Binary format (single file)
        'use_selection': export_selection_only,
        'export_apply': apply_modifiers,
    }

    # Animation export options
    if export_animations:
        export_options['export_animations'] = True
        export_options['export_frame_range'] = True
        export_options['export_force_sampling'] = True
        print("Animation export enabled")

        # List animations if any
        if bpy.data.actions:
            print(f"Found {len(bpy.data.actions)} animation(s):")
            for action in bpy.data.actions:
                print(f"  - {action.name}, frames: {int(action.frame_range[0])} - {int(action.frame_range[1])}")
        else:
            print("Warning: No animations found")

        # Configure root motion removal for in-place animations
        if remove_root_motion_flag:
            # Use glTF's built-in slide to zero feature
            export_options['export_anim_slide_to_zero'] = True
            print("Root motion removal enabled (export_anim_slide_to_zero=True)")

    # Compression options
    if use_compression:
        export_options['export_draco_mesh_compression_enable'] = True
        export_options['export_draco_mesh_compression_level'] = compression_level
        print(f"Draco compression enabled (level {compression_level})")

    # Coordinate system conversion
    export_options['export_yup'] = export_yup
    if not export_yup:
        print("Preserving Z-up orientation (export_yup=False)")
    else:
        print("Converting to Y-up orientation (export_yup=True)")

    # Export to GLB
    print("=== EXPORTING TO GLB ===")
    bpy.ops.export_scene.gltf(**export_options)
    print("=== EXPORT COMPLETE ===")

    # Verify export and report file size
    if os.path.exists(output_glb_path):
        size_kb = os.path.getsize(output_glb_path) / 1024
        print(f"✅ Successfully exported to: {output_glb_path}")
        print(f"   File size: {size_kb:.1f} KB")
        return True
    else:
        print("❌ ERROR: Export failed, file not created")
        return False


def main():
    """Parse command line arguments and run export."""
    # Parse command line arguments
    # Blender passes args after '--' to the script
    argv = sys.argv
    if "--" in argv:
        argv = argv[argv.index("--") + 1:]
    else:
        print("Usage: blender --background --python export_to_glb.py -- <input_blend> <output_glb> [options]")
        print("\nOptions:")
        print("  --compress              Enable Draco mesh compression")
        print("  --compression-level N   Compression level 0-10 (default: 6)")
        print("  --animations            Export animations")
        print("  --selection-only        Export only selected objects")
        print("  --no-apply-modifiers    Don't apply modifiers before export")
        print("  --scale N               Scale factor for mesh and bones (default: 1.0)")
        print("  --no-yup                Preserve Z-up orientation (default: convert to Y-up)")
        sys.exit(1)

    if len(argv) < 2:
        print("ERROR: Missing arguments")
        print("Usage: blender --background --python export_to_glb.py -- <input_blend> <output_glb> [options]")
        sys.exit(1)

    # Required arguments
    input_blend = argv[0]
    output_glb = argv[1]

    # Optional arguments
    use_compression = "--compress" in argv
    export_animations = "--animations" in argv
    export_selection_only = "--selection-only" in argv
    apply_modifiers = "--no-apply-modifiers" not in argv
    export_yup = "--no-yup" not in argv
    apply_rotation_fix = "--no-rotation-fix" not in argv
    remove_root_motion_flag = "--remove-root-motion" in argv

    # Parse compression level
    compression_level = 6
    if "--compression-level" in argv:
        idx = argv.index("--compression-level")
        if idx + 1 < len(argv):
            try:
                compression_level = int(argv[idx + 1])
                if not 0 <= compression_level <= 10:
                    print("Warning: Compression level must be 0-10, using default (6)")
                    compression_level = 6
            except ValueError:
                print("Warning: Invalid compression level, using default (6)")

    # Parse scale factor
    scale_factor = 1.0
    if "--scale" in argv:
        idx = argv.index("--scale")
        if idx + 1 < len(argv):
            try:
                scale_factor = float(argv[idx + 1])
                if scale_factor <= 0:
                    print("Warning: Scale factor must be positive, using default (1.0)")
                    scale_factor = 1.0
            except ValueError:
                print("Warning: Invalid scale factor, using default (1.0)")

    print(f"Input:  {input_blend}")
    print(f"Output: {output_glb}")
    print(f"Options: compression={use_compression}, animations={export_animations}, "
          f"selection_only={export_selection_only}, apply_modifiers={apply_modifiers}, "
          f"scale={scale_factor}, export_yup={export_yup}, apply_rotation_fix={apply_rotation_fix}, "
          f"remove_root_motion={remove_root_motion_flag}")

    success = export_to_glb(
        input_blend,
        output_glb,
        use_compression=use_compression,
        compression_level=compression_level,
        export_animations=export_animations,
        export_selection_only=export_selection_only,
        apply_modifiers=apply_modifiers,
        scale_factor=scale_factor,
        export_yup=export_yup,
        apply_rotation_fix=apply_rotation_fix,
        remove_root_motion_flag=remove_root_motion_flag
    )

    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
