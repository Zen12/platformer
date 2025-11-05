## 📋 Requirements
- 🧠 ```C++17```
- 🛠️ ```Cmake``` >3.28
- ⚙️ ```[optional]``` Python used for editor tools
- 🛠️ ```[optional]``` Emscripten used for webgl build
## 👋 Getting started 🤞
- 🏃```run.sh```
### 🛠️ Webgl build
https://andreigusan.itch.io/platformer-demo

## 🔨 Project structure
``` 
⚙️ .gitignore 
🗂️ assets               # engine-project used in build 
    ├─ 🗂️ resources
    └─ 🗒️ project.yaml
📦 bin                  # build folder for ./run.sh
📚 cmake                # packages
    ├─ ⚙️ FindBox2D.cmake
    ├─ ⚙️ FindFreetype.cmake
    ├─ ⚙️ FindGLEW.cmake
    ├─ ⚙️ FindGLFW3.cmake
    ├─ ⚙️ FindGLM.cmake
    ├─ ⚙️ FindSpine.cmake
    └─ ⚙️ FindYamlCpp.cmake
🛠️ editor               # editor scripts, not real editor
    ├─ 📄 import.py
    └─ 📄 clear_all_refs.py
💻 src                  # source code
    ├─ 🚀 main.cpp
    └─ 🧠 engine
        ├─ 📄 engine.h
        ├─ 📁 assets
        ├─ 📁 components
        ├─ 📁 renderers
        ├─ 📁 render_pipiline
        └─ 📁 system
🚀 run.sh               # release build desktop
🚀 run_debug.sh         # debug build desktop
🚀 run_web.sh           # release build webgl
⚙️ CMakeLists.txt
📘 readme.md
 ```

## 🦾 Features
- 🏢 macOS, windows, web
- 🔳 UI system
- 📀 Asset management
- 🔦 2d lightning
- 🎳 2d physics

# 🏃 Roadmap

## 0.1.2 From OOP to data oriented(DOD)
- [x] Set Entt
- [x] Redo components to Entt
- [ ] Redo systems (rendering, physics, etc...)

## 0.1.3 FSM
- [ ] FSM for animation and app-state 
- [ ] Create AnimationComponent
- [ ] Create appstate flow 

## 0.1.4 Hot reload
- [ ] Split into 2 cmake project (app + engine)
- [ ] Compile engine into dll/so
- [ ] Implement hot-reload (swapping dll and reloading it)
- [ ] Split Engine logic and game logic

## 0.1.5 First Editor
- [ ] Create new cmake project (editor)
- [ ] ImGUI for editor 
- [ ] Basic serialization viewer with play button

## Backlog
- [ ] Proper fix retina issue on webgl and macOS
- [ ] Optimization of light raycast system, the vertex approach

🇲🇩