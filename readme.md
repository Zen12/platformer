## 📋 Requirements
- 🧠 ```C++17```
- 🛠️ ```Cmake``` 3.28 (<4.0)
- ⚙️ ```[optional]``` Python used for editor tools
- 🛠️ ```[optional]``` Emscripten used for webgl build
## 👋 Getting started 🤞
### 🍎 macOS 
- 🏃```run.sh``` 
### 🪟 Windows 
TODO
### 🐧 Linux 
TODO
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

## 📚 Code convention
- ```ClassOrStructName```
- ```PublicVariable```
- ```MethodName()```
- ```FunctionName()```
- ```_privateOrProtectedVariable```
- ```localVariable```
- ```CONST_VARIALBE```
- ```file_name.cpp/hpp```
- ```folder_name```
- ```serialized_file_in_yaml```
- ```//comments``` means hack-description/todo or created by AI

## 🛠️ Architecture
### 📀 Memory management
- Lifetime of an obj is determined by ```shared_ptr```/```week_ptr```
- Raw pointers can be used for 3rd party api calls
- ```Engine``` is the owner of ```Managers```
- ```Managers``` are the owners of ```Assets``` and ```Entities```
- ```Entity``` is owner of ```Component```
- The destruction flow:
  - ```~Engine``` -> ```~Manager``` -> ```~Asset``` / ```~Entity``` -> ```~Component```

### 🔦 Rendering
- Only 2d support
- Call order: ```Engine``` -> ```RenderPipeline``` -> ```RenderComponent``` ->  ```Mesh```/```Material```/```Asset```
- Each render type has it's own camera. They should be as considered independent concepts 
- ```Renderers``` has only one ```material```. ```Material``` is data + ```Shader```
- Some renderers can have data separate from Material
- Types of renders:
  - Ui:
    - ```UiImage```
    - ```UiText```
  - 3d World:
    - ```SpriteRenderer```:
      - Used as background
      - Are not sorted by Z (fix later)
    - ```MeshRenderer```:
      - Used for most important (Light2d, Spine)
      - Sorted by Z
      - SpineRender updates ```MeshRenderer```
- World coordinates:
  - UI
    - ```Projection``` is Camera ortho based of window size
    - ```UiLayouts``` alter the ```Model``` matrix for screen responsiveness
  - 3d World:
    - Model-View-Projection
    - ```RenderPipline``` orders by Z only ```MeshRenderers```
### 🗂️ Asset management
- Yaml serialization
- File path agnostic, generates .meta files for reference tracking

### 🧠 What is missing
- Proper editor. ATM, just temporary python scripts
- Proper FSM for App-State management and animation state behaviour
- Sound

# 🏃 Roadmap

## 0.1.1 Refactor + Optimization
- [x] Simplified yaml
- [ ] Reorganize factory/serializable classes
- [ ] Refactor spine loader
- [ ] Refactor AIController and CharacterController

## 0.1.2 From OOP to data oriented(DOD)
- [ ] Set Entt
- [ ] Redo components to Entt
- [ ] Redo systems (rendering, physics, etc...)

## 0.1.3 FSM
- [ ] FSM for animation and app-state 
- [ ] Create AnimationComponent 
- [ ] Create renderer base class and unify all renderers
- [ ] Create appstate flow 

## 0.1.4 Hot reload
- [ ] Split into 2 cmake project (app + engine)
- [ ] Compile engine into dll/so
- [ ] Implement hot-reload (swapping dll and reloading it)
- [ ] Split Engine logic and game logic

## 0.1.5 First Editor
- [ ] Fix model matrix, multiplication order
- [ ] Create new cmake project (editor)
- [ ] Refactor engine to render into render-texture
- [ ] ImGUI for editor + rendering into render-texture
- [ ] Basic serialization viewer with play button
- [ ] Test build on windows and linux
- [ ] Proper fix retina issue on webgl and macOS

## Backlog
- [ ] Optimization of light raycast system, the vertex approach
- [ ] Move to c++20 to use mudules

🇲🇩