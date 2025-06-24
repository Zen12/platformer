## 📋 Requirements
- 🧠 ```C++17```
- 🛠️ ```Cmake``` 3.28 (<4.0)
- 🛠️ 🚀 ```FetchContent_Declare``` is used as package manager
- ⚙️ ```[optional]``` Python used for editor tools
## 👋 Getting started 🤞
### 🍎 macOS 
- 🏃```run.sh``` ⌛```1-2 minutes``` 
### 🪟 Windows 
TODO
### 🐧 Linux 
TODO

## 🔨 Project structure 

``` 
⚙️ .gitignore 
🗂️ assets
    ├─ 🗂️ resources
    └─ 🗒️ project.yaml
📦 bin
📦 build
📚 cmake 
    ├─ ⚙️ FindFreetype.cmake
    ├─ ⚙️ FindGLEW.cmake
    ├─ ⚙️ FindGLFW3.cmake
    ├─ ⚙️ FindGLM.cmake
    └─ ⚙️ FindYamlCpp.cmake
🛠️ editor_tools 
    └─ 📄 import.py
💻 src 
    ├─ 🚀 main.cpp
    └─ 🧠 src
        ├─ 📄 engine.h
        ├─ 📁 assets
        ├─ 📁 components
        ├─ 📁 renderers
        ├─ 📁 render_pipiline
        └─ 📁 system
🚀 run.sh
🚀 run_debug.sh
⚙️ CMakeLists.txt
📘 readme.md
 ``` 



## 🦾 Features 
- 🎨 OpenGL/WebGL
- 🏢 macOS, windows, web
- 🔳 UI system 
    - ⚓ Anchor base 
    - 🈸 Font rendering
- 📀 Asset management 
    - 🖥️ Small Editor
    - 📚 Scene
- 🔦 2d lightning
- 🌙 Lua scripting

## TODO
- [x] Input system
- [ ] Collider component
- [ ] Rigidbody component
- [ ] Light2d
- [ ] 2d Character controller
- [ ] Script component (Lua)
- [ ] Webgl build 
- [ ] UI buttons
- [ ] Post-processing components
- [ ] Tests
- [ ] Editor 
- [ ] FSM/Behaviour-tree


🇲🇩