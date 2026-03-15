# Platformer Engine

Custom C++17 game engine with plugin-based architecture, ECS, finite state machines, skeletal animation, navmesh pathfinding, and AI-assisted development via MCP.

![Demo](docs/readme/demo.gif)

[Play the demo on itch.io](https://andreigusan.itch.io/platformer-demo)

---

## Architecture Highlights

- **Plugin system** — 17 modular plugins (renderer, animation, audio, navigation, AI, IK) registered via template-based APIs with zero boilerplate
- **ECS** — EnTT-based entity component system with YAML-driven scene serialization and factory pattern for components
- **FSM** — Finite state machines driving game flow, UI navigation, and animation states with variant-based action dispatch
- **Asset pipeline** — GUID-based asset management with .meta files, reference tracking, lazy loading, and cache system
- **MCP integration** — AI-assisted development with Claude, including Blender, GIMP, and Figma tool servers

## Features

| System | Details |
|--------|---------|
| **Rendering** | OpenGL 3.3+, material abstraction, instanced rendering (per mesh+material), frustum culling, shadow mapping, post-processing |
| **Skinned mesh** | Skeletal animation with bone support, inverse kinematics, instanced skinned rendering, FSM-driven animation states |
| **Navigation** | 2D navmesh builder, A* pathfinding, RVO crowd simulation |
| **AI** | Behavior trees with blackboard state |
| **UI** | RmlUi page system with hooks, data-binding, GUID-based asset references |
| **Audio** | OpenAL with 3D spatial sound, one-shot and loop clips |
| **Particles** | Lifetime, color |
| **Platforms** | macOS, WebGL (Emscripten) |

## Getting Started

### Requirements
- C++17 compiler
- CMake 3.11+
- _Optional:_ Python (for MCP tools), Emscripten (for WebGL build)

### Build and Run
```bash
./run.sh              # release build (desktop)
./run_debug.sh        # debug build (desktop)
./run_web.sh          # WebGL build
./run_claude.sh       # AI-assisted development with Claude
```

## Documentation

- [FSM System](docs/fsm-system.md) — Finite state machine architecture and workflow
- [UI System](docs/ui-system.md) — RmlUi integration and FSM-driven UI
- [Material System](docs/material.md) — Material creation and shader workflow
- [Blender Integration](docs/blender.md) — 3D modeling and GLB export pipeline
- [MCP Setup](docs/mcp.md) — Model Context Protocol and asset tools
- [Instancing System](docs/instancing-system.md) — Instanced rendering architecture
- [Skinned Mesh Workflow](docs/skinned-mesh-workflow.md) — Character animation pipeline

## Project Structure

```
src/
    ├── main.cpp               # entry point
    └── engine/
        ├── core/              # asset manager, ECS, FSM, plugin registry
        ├── plugins/           # 17 modular plugins
        │   ├── renderer/      # OpenGL rendering, UI, materials
        │   ├── animation/     # skeletal animation
        │   ├── audio/         # OpenAL audio
        │   ├── navigation/    # navmesh, A*, RVO
        │   ├── ai/            # behavior trees
        │   ├── ik/            # inverse kinematics
        │   ├── scene/         # scene loading
        │   └── ...
        └── examples/          # plugin example with tests
assets/
    ├── resources/             # game assets (models, scenes, UI, shaders)
    └── project.yaml           # project configuration
mcp_tools/                     # Python MCP servers (asset, blender, gimp, figma)
```

## Roadmap

- **Lighting** — Dynamic light meshes with shadows, decal system
- **Gameplay** — IK-driven shooting, particle effects for combat
- **Engine** — Sub-FSM support, prefab system with nesting, audio mixer, Navmesh raycasting
- **Platforms** — Windows and Linux support

🇲🇩
