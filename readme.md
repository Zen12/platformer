## 📋 Requirements
- 🧠 ```C++17```
- 🛠️ ```Cmake``` >3.28
- 🤖 ```[optional]``` Python used for MCP
- 🛠️ ```[optional]``` Emscripten used for webgl build
## 👋 Getting started 🤞
- 🏃```run.sh```

## 🤖 MCP (Model Context Protocol)

AI-assisted asset management for GUID-based references, metadata generation, and safe deletion.

**Quick start:**
```bash
./run_claude.sh
```

**See [docs/mcp.md](docs/mcp.md) for detailed documentation.**

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
🚀 run_debug.sh         # debug build desktop
🚀 run_web.sh           # release build webgl
⚙️ CMakeLists.txt
📘 readme.md
 ```

## 🦾 Features
- 🏢 macOS, windows, web
- 🔳 UI system (HTML/CSS-like)
- 📀 Asset management
- 🚀 ESC
- 🔦 Basic mesh 3d rendering
- 🧠 Nav mesh
- 🔄 FSM
- 🏄 MCP

# 🏃 Roadmap

## 0.1.4 Model Context Protocol (MCP)
- [ ] Python scripts for fsm
- [ ] Python scripts for import
- [ ] Python scripts for ui
- [ ] Python scripts for shader/materials/images

## 0.1.5 Mesh Rendering
- [ ] Mesh Loader (FBX??, glTF??, .blend??)
- [ ] Frustum Culling
- [ ] IK/Animations/Skinned Mesh

## 0.1.6 NavMesh
- [ ] NavMesh builder (floating and fix point), grid based for determinism
- [ ] Astar navigation (path query with smooth path)

## 0.1.7 3d platformer game
- [ ] Behaviour Tree
- [ ] One level of 3d platformer ("Squirrel with a gun"-like) with basic AI

## Backlog
- [ ] Proper fix retina issue on webgl and macOS

🇲🇩