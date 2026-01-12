#!/bin/bash
# Quick export ground2.blend to GLB with baked transforms (centered at origin)
DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$DIR/../../../.."

SCALE_FACTOR=1.025

blender --background "$DIR/original/ground2.blend" --python-expr "
import bpy

SCALE_FACTOR = $SCALE_FACTOR

for obj in list(bpy.data.objects):
    if obj.type == 'MESH':
        print(f'Processing: {obj.name}')
        print(f'  Location: {obj.location[:]}')
        print(f'  Scale: {obj.scale[:]}')
        print(f'  Extra scale factor: {SCALE_FACTOR}')

        scale = obj.scale.copy()
        old_mesh = obj.data

        # Create a new mesh with scaled vertices
        new_mesh = old_mesh.copy()
        new_mesh.name = 'Ground_baked'

        for vert in new_mesh.vertices:
            vert.co.x *= scale.x * SCALE_FACTOR
            vert.co.y *= scale.y * SCALE_FACTOR
            vert.co.z *= scale.z

        # Create new object with the baked mesh
        new_obj = bpy.data.objects.new('Ground', new_mesh)
        bpy.context.collection.objects.link(new_obj)

        # Remove old object
        bpy.data.objects.remove(obj)

        # Report bounds
        xs = [v.co.x for v in new_mesh.vertices]
        ys = [v.co.y for v in new_mesh.vertices]
        zs = [v.co.z for v in new_mesh.vertices]
        print(f'  Baked bounds: X({min(xs):.1f} to {max(xs):.1f}), Y({min(ys):.1f} to {max(ys):.1f}), Z({min(zs):.1f} to {max(zs):.1f})')

bpy.ops.export_scene.gltf(filepath='$DIR/ground2.glb', export_format='GLB', export_yup=True, export_skins=False, export_animations=False)
print('Exported: $DIR/ground2.glb')
" 2>&1 | grep -v "Not freed"

python3 "$ROOT/mcp_tools/import.py" "$DIR/ground2.glb"
echo "Done!"
