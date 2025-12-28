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
- 🏢 macOS, windows, web
- 🔳 UI system (HTML/CSS-like)
- 📀 Asset management
- 🚀 ESC
- 🔦 Basic mesh 3d rendering
- 🧠 Nav mesh
- 🔄 FSM
- 💫 MCP

## 🔨 Project structure
``` 
⚙️ .gitignore 
🗂️ assets               # dummy project example
    ├─ 🗂️ resources
    └─ 🗒️ project.yaml
📦 bin                  # build folder for ./run.sh
📚 cmake                # packages
    ├─ ⚙️ Find***.cmake
🛠️ mcp              
💻 src                  # source code
    ├─ 🚀 main.cpp      # entry point 
    └─ 🧠 engine
        ├─ 📄 engine.hpp
        ├─ 📁 **CORE**
🚀 run.sh               # release build desktop
🐛 run_debug.sh         # debug build desktop
🌐 run_web.sh           # release build webgl
💫 run_claude.sh        # setup and run claude
⚙️ CMakeLists.txt
📘 readme.md
 ```

# 🏃 Roadmap

## 0.1.4 Model Context Protocol (MCP)
- [x] Python scripts for fsm
- [x] Python scripts for import
- [x] Python scripts for ui
- [x] Python scripts for shader/materials/images

## 0.1.5 Mesh Rendering
- [x] Mesh Loader (GLB format via Assimp)
- [x] Coordinate system conversion (Blender Y-up to Engine Y-forward)
- [x] Debug logging and performance timing
- [x] Frustum Culling
- [x] Animations/Skinned Mesh
- [ ] Link animation to FSM

## 0.1.6 NavMesh
- [ ] NavMesh builder (floating and fix point), grid based for determinism, backed
- [ ] Astar navigation (path query with smooth path)

## 0.1.7 3d platformer game
- [ ] Behaviour Tree as ESC
- [ ] Horde-skinned-instance rendering
- [ ] One level of 3d platformer with horde AI(Alien shooter?)

## Backlog
- [ ] Proper fix retina issue on webgl and macOS
- [ ] Proper bounce component or part of renderer, need to filet by bounds and not hardcoded vals

🇲🇩