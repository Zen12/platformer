## 📋 Requirements
- 🧠 ```C++17```
- 🛠️ ```Cmake``` >3.28
- ⚙️ ```[optional]``` Python used for editor tools
- 🛠️ ```[optional]``` Emscripten used for webgl build
## 👋 Getting started 🤞
- 🏃```run.sh```

## 🔨 Project structure
``` 
⚙️ .gitignore 
🗂️ assets               # dummy project example
    ├─ 🗂️ resources
    └─ 🗒️ project.yaml
📦 bin                  # build folder for ./run.sh
📚 cmake                # packages
    ├─ ⚙️ Find***.cmake
🛠️ editor               # editor scripts, not real editor
    ├─ 📄 import.py
    └─ 📄 clear_all_refs.py
💻 src                  # source code
    ├─ 🚀 main.cpp
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
- 🔳 UI system
- 📀 Asset management
- 🚀 ESC
- 🔦 Basic mesh 3d rendering 
- 🧠 Nav mesh

# 🏃 Roadmap

## 0.1.3 FSM
- [ ] 3d renderer manager
- [ ] Create appstate flow (FSM)
- [ ] Mesh Loader

## 0.1.4 First Editor
- [ ] Create new cmake project (editor)
- [ ] ImGUI for editor 
- [ ] Basic serialization viewer with play button

## Backlog
- [ ] Proper fix retina issue on webgl and macOS

🇲🇩