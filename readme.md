## 📋 Requirements
- 🧠 ```C++17```
- 🛠️ ```Cmake``` 3.11+
- 💫 ```[optional]``` Python used for MCP
- 🌐 ```[optional]``` Emscripten used for webgl build

## 👋 Getting started 🤞
- 🏃```run.sh```
- 💫```run_claude.sh``` [docs/mcp.md](docs/mcp.md)

## 👀 Demo

![Demo](docs/readme/demo.gif)

## 🦾 Features
- 🏢 Platform
  - [x] macOS
  - [x] web
  - [ ] windows (0.3+)
  - [ ] linux (0.3+)
- 🔳 UI system 
  - [x] Page system
  - [x] Hooks
  - [x] Data-binding
- 📀 Asset management
  - [x] Lazy load 
  - [x] .Meta file support
  - [ ] Ref count (0.2+)
- 🚀 ESC
  - [x] Single-thread support
  - [ ] Multi-thread support (0.3+)
- 🔦 Basic mesh 3d rendering
  - [x] Material abstraction
  - [x] Instance rendering (per mesh+material)
  - [x] Frustum culling
  - [ ] Decals system
- 💃 Skinned mesh rendering 
  - [x] Bones support
  - [x] Animation via state (FSM)
  - [ ] IK (0.2+)
  - [x] Instance rendering
- 🧠 Nav mesh
  - [x] 2d builder
  - [x] AStar
  - [x] RVO
- 🔄 FSM 
  - [x] Node support for multiple actions
  - [x] Scene management Action
  - [x] UI management Action
  - [x] Conditions
  - [ ] SubFSM (0.3+)
- 💫 MCP
  - [x] Claude support
  - [x] Asset management with .meta
  - [x] Import/export systems for .blend and .glb
  - [x] Hooks to blender and gimp mcp
  - [x] Scene awareness (fast scene builder)
  - [ ] File lock system for multiple agents (0.3+)
- 🔊Sound
  - [x] One shot clip
  - [x] Loop clip
  - [ ] Mixer (0.2+)
- 💠Prefab 
  - [x] Entity prefab
  - [ ] Nested prefab (0.2+)
- ▶️Editor (0.2+)
  - [x] Gameplayer recorder
  - [ ] Scene visualize 
- 💥Particle System
  - [x] Lifetime 
  - [x] Color
  - [ ] Shape (0.2+)

## 📚 Documentation
- [MCP Setup](docs/mcp.md) - Model Context Protocol and asset tools
- [Blender Integration](docs/blender.md) - 3D modeling with Blender MCP
- [Material System](docs/material.md) - Material creation and shader workflow
- [UI System](docs/ui-system.md) - RmlUi UI system and FSM integration
- [FSM System](docs/fsm-system.md) - Finite State Machine architecture and workflow

 
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

## 0.1.9 Good level
- [ ] Mesh builder from navmesh grid
- [ ] Walls generator 
- [ ] Texture applier (floor, walls)

## 0.1.10 Lighting of level
- [ ] Fog 
- [ ] 3d light generator as mesh (with shadow)
- [ ] Decal system

## 0.1.11 Animation and Gameplay
- [ ] IK for shooting
- [ ] Shooting behaviour
- [ ] Kill all zombies condition
- [ ] Particle system for shooting

## 0.1.12 Small polish
- [ ] Background for main menu
- [ ] Win/Lose screen 

## Backlog
- [ ] Proper fix retina issue on webgl and macOS
- [ ] Proper AssetManager (not link to scene), some refcount solution
- [ ] Proper Particle system 

🇲🇩