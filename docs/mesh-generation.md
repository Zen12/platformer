# Mesh Generation System

## Overview

The mesh generation system creates procedural meshes at runtime from `.meshgen` YAML asset files. It supports CSG primitives (cube, sphere, cylinder), bezier tube extrusion, and boolean operations (union, difference, intersection) via the [Manifold](https://github.com/elalish/manifold) library.

Generated meshes integrate transparently with the renderer — reference a `.meshgen` GUID wherever you'd use a `.glb` GUID.

## Architecture

```
gen_mesh_renderer component (holds .meshgen GUID)
        │
        ▼
ResourceFactory::Get<Mesh>(guid)
        │
        ├── cache hit? → return cached mesh
        │
        ├── check MetaAsset type
        │   type == "meshgen"? → registered generator callback
        │       │
        │       ▼
        │   MeshGenAsset (YAML) → MeshGenerator::Generate()
        │       │
        │       ▼
        │   Manifold CSG operations → vertices + indices
        │       │
        │       ▼
        │   Mesh created → cached (never regenerated)
        │
        └── else → Assimp loader (existing .glb path)
```

Key files:
- `src/engine/plugins/mesh_generator/src/mesh_generator.hpp` — core CSG logic
- `src/engine/plugins/mesh_generator/src/meshgen_resource_generator.hpp` — registers generator callback
- `src/engine/plugins/mesh_generator/src/meshgen_asset.hpp` — asset structs
- `src/engine/plugins/bezier/src/bezier_curve.hpp` — bezier sampling utilities

## Asset Format

### `.meshgen` file

```yaml
name: my_mesh
operations:
  - type: <operation_type>
    <params...>
  - type: <operation_type>
    <params...>
```

Operations are evaluated in order. Each produces a result stored by index (0, 1, 2, ...). The **last operation** is the final mesh output.

### `.bezier` file

```yaml
name: my_curve
control_points:
  - [0.0, 0.0, 0.0]   # P0 (start)
  - [1.0, 2.0, 0.0]   # P1 (control)
  - [3.0, 2.0, 0.0]   # P2 (control)
  - [4.0, 0.0, 0.0]   # P3 (end)
```

Cubic bezier segments need 4 points each. Multiple segments use 3n+1 points (4, 7, 10, ...) where segments share endpoints.

## Operation Types

### Primitives

All primitives support optional transform parameters: `translate`, `rotate` (Euler degrees), `scale`.

#### `cube`

```yaml
- type: cube
  size: [2.0, 1.0, 3.0]    # width, height, depth (default: [1,1,1])
  translate: [0.0, 0.5, 0.0]  # optional
```

Centered at origin by default.

#### `sphere`

```yaml
- type: sphere
  radius: 0.8               # default: 1.0
  segments: 32               # tessellation (default: 16)
```

#### `cylinder`

```yaml
- type: cylinder
  radius: 0.5               # default: 1.0
  height: 2.0               # default: 1.0
  segments: 16               # default: 16
```

Centered at origin, extends along Y axis.

### Bezier Tube Extrusion

#### `bezier_extrude`

Sweeps a circular cross-section along a bezier curve path.

```yaml
- type: bezier_extrude
  bezier_guid: <guid-of-.bezier-file>
  height: 0.4               # tube diameter (default: 1.0)
  segments: 24               # path samples AND circle segments (default: 16)
```

- `height` controls tube diameter (radius = height / 2)
- `segments` controls path sampling resolution; circle segments = max(segments/2, 6)
- Produces a watertight manifold mesh with capped ends
- Uses Frenet frames (tangent/normal/binormal) for cross-section orientation

### Boolean Operations

Combine two or more previous operation results using CSG.

#### `union`

Merges shapes together.

```yaml
- type: union
  operands: [0, 1]          # indices of previous operations
```

#### `difference`

Subtracts subsequent operands from the first.

```yaml
- type: difference
  operands: [0, 1]          # result[0] minus result[1]
```

#### `intersection`

Keeps only the overlapping volume.

```yaml
- type: intersection
  operands: [0, 1]
```

All boolean operations require at least 2 operands. Multiple operands are applied sequentially: `((op[0] OP op[1]) OP op[2]) OP ...`

## Complete Example

Bezier tube with a cube cut out of it:

```yaml
# test_mesh.meshgen
name: test_generated_mesh
operations:
  - type: bezier_extrude          # index 0
    bezier_guid: eebf54c2-4cc6-46ac-8884-d18c91e8477e
    height: 0.4
    segments: 24
  - type: cube                    # index 1
    size: [0.5, 0.5, 0.5]
    translate: [1.0, 1.3, 0.0]
  - type: difference              # index 2 (final output)
    operands: [0, 1]
```

## Using in a Scene

Reference the `.meshgen` GUID in a `gen_mesh_renderer` component:

```yaml
- name: generated_mesh
  components:
    - type: transform
      position: [0.0, 0.0, 0.0]
    - type: gen_mesh_renderer
      mesh: <meshgen-guid>        # GUID from .meshgen.meta file
      material: <material-guid>   # any existing material
```

Use `gen_mesh_renderer` (not `mesh_renderer`) for generated meshes. The system handles mesh resolution and caching automatically.

## Operation Parameters Reference

| Parameter | Type | Default | Used By |
|-----------|------|---------|---------|
| `type` | string | required | all |
| `size` | vec3 | [1,1,1] | cube |
| `radius` | float | 1.0 | sphere, cylinder |
| `height` | float | 1.0 | cylinder, bezier_extrude |
| `segments` | int | 16 | sphere, cylinder, bezier_extrude |
| `translate` | vec3 | [0,0,0] | primitives, bezier_extrude |
| `rotate` | vec3 | [0,0,0] | primitives, bezier_extrude (Euler degrees) |
| `scale` | vec3 | [1,1,1] | primitives, bezier_extrude |
| `bezier_guid` | GUID | — | bezier_extrude |
| `operands` | int[] | — | union, difference, intersection |

## Creating Assets

1. Create `.bezier` file in `assets/resources/curves/`
2. Create `.meshgen` file in `assets/resources/meshgen/`
3. Run `generate_asset_metadata()` MCP tool to create `.meta` files
4. Reference the `.meshgen` GUID in a scene entity with `gen_mesh_renderer`
5. Build with `./run_debug.sh`
