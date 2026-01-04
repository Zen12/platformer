## 📋 Requirements
- 🧠 ```C++17```
- 🛠️ ```Cmake``` 3.11+
- 💫 ```[optional]``` Python used for MCP
- 🌐 ```[optional]``` Emscripten used for webgl build

## 👋 Getting started 🤞
- 🏃```run.sh```
- 💫```run_claude.sh``` [docs/mcp.md](docs/mcp.md)

## 📚 Documentation
- [MCP Setup](docs/mcp.md) - Model Context Protocol and asset tools
- [Blender Integration](docs/blender.md) - 3D modeling with Blender MCP
- [Material System](docs/material.md) - Material creation and shader workflow
- [UI System](docs/ui-system.md) - RmlUi UI system and FSM integration
- [FSM System](docs/fsm-system.md) - Finite State Machine architecture and workflow

## 🦾 Features
- 🏢 Platform
  - [x] macOS
  - [x] web
  - [ ] windows (0.2+)
  - [ ] linux (0.2+)
- 🔳 UI system 
  - [x] Page system
  - [x] Hooks
  - [ ] Data-binding
- 📀 Asset management
  - [x] Lazy load 
  - [x] .Meta file support
  - [ ] Ref count (0.2+)
- 🚀 ESC
  - [x] Single-thread support
  - [ ] Multi-thread support (0.2+)
- 🔦 Basic mesh 3d rendering
  - [x] Material abstraction
  - [x] Instance rendering (per mesh+material)
  - [x] Frustum culling
  - [ ] PBR
- 💃 Skinned mesh rendering 
  - [x] Bones support
  - [x] Animation via state (FSM)
  - [ ] IK (0.2+)
  - [ ] Instance rendering (0.2+)
- 🧠 Nav mesh
  - [ ] 2d builder
  - [ ] AStar
  - [ ] RVO
- 🔄 FSM 
  - [x] Node support for multiple actions
  - [x] Scene management Action
  - [x] UI management Action
  - [x] Conditions
  - [ ] SubFSM (0.2+)
- 💫 MCP
  - [x] Claude support
  - [x] Asset management with .meta
  - [x] Import/export systems for .blend and .glb
  - [x] Hooks to blender and gimp mcp
  - [x] Scene awareness (fast scene builder)
  - [ ] File lock system for multiple agents (0.2+)
- 🔊Sound
  - [ ] One shot clip
  - [ ] Loop clip
  - [ ] Mixer (0.2+)
- 💠Prefab (0.2+)
  - [ ] Entity prefab
  - [ ] Nested prefab
- ▶️Editor (0.2+)
  - [x] Gameplayer recorder
  - [ ] Scene visualize 
  - [ ] MCP as Editor
- 💥Particle System (0.2+)

 
## 🔨 Project structure
``` 
⚙️ .gitignore 
🗂️ assets               # dummy project example
    ├─ 🗂️ resources
    └─ 🗒️ project.yaml
📦 bin                  # build folder for ./run.sh
📚 cmake                # packages
    └─ ⚙️ Find***.cmake
🛠️ mcp_tools              
💻 src                  # source code
    ├─ 🚀 main.cpp      # entry point 
    └─ 🧠 engine
        ├─ 📄 engine.hpp
        ├─ 📁 **CORE**
🚀 run.sh               # release build desktop
🐛 run_debug.sh         # debug build desktop
🌐 run_web.sh           # release build webgl
💫 run_claude.sh        # setup and run claude
▶️ run_record.sh        # setup and record video
⚙️ CMakeLists.txt
📘 readme.md
 ```

# 🏃 Roadmap

## 0.1.6 NavMesh
- [x] Video recorder 
- [x] NavMesh builder, grid bitased
- [x] Astar navigation (path query with smooth path) + RVO

## 0.1.7 3d platformer game
- [ ] Behaviour Tree as ESC
- [ ] Proper bounce component or part of renderer, need to filet by bounds and not hardcoded vals
- [ ] One level of 3d platformer with horde AI

## 0.1.8 Basic polish
- [ ] Simple particles
- [ ] Simple audio
- [ ] PBR + shadows
- [ ] Basic UI (+ data-bindings)

## Backlog
- [ ] Proper fix retina issue on webgl and macOS
- [ ] Instance rendering for skinned mesh (per mesh+material)
- [ ] Proper AssetManager (not link to scene), some refcount solution
- [ ] Proper Audio system
- [ ] Proper Particle system 

🇲🇩