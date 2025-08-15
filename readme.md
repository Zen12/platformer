## 📋 Requirements
- 🧠 ```C++17```
- 🛠️ ```Cmake``` 3.28 (<4.0)
- 🛠️ 🚀 ```FetchContent_Declare``` is used as package manager
- ⚙️ ```[optional]``` Python used for editor tools
## 👋 Getting started 🤞
### 🍎 macOS 
- 🏃```run.sh``` 
### 🪟 Windows 
TODO
### 🐧 Linux 
TODO

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
🚀 run.sh               # release build script
🚀 run_debug.sh         # debug build script
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
- ```//comments``` means hack-description or todo

## 📚 Class types(Suffixes)

- ```Manager``` controls life-time and execution of objects
- ```Data``` data that is generated at runtime, used by ```Components```
- ```Asset``` files that are used to generate ```Data```
- ```Factory``` creates ```Data``` or ```Component```
- ```ComponentSerialization``` DTO from ```Asset``` to ```Component```
- ```Renderer``` render things
- ```Component``` Has simple logic to update ```Renderer``` or other ```Component```
- ```Controller``` Has complex logic that involves interaction with ```System``` or/and ```Controllers``` or/and ```Components```
- ```System``` something to interact with native-os or 3rd party. Is NOT part of the ```Scene```

## 📚 Class structure
```

class MyClass:
{
// 1st public variables 
public:
  int MyPublicVariable;
  
// 2nd protected variables
protected:
  int _myProtectedVariable;

// 3rd private variables
private:
  int _myPrivateVariable;
  
  
// 4th public methods
public:
  void MyPublicMethod();
  
// 5th protected methods
protected:
  void MyProtectedMethod();
  
// 6th protected methods
protected:
  void MyPrivatedMethod();

}
```

## 🛠️ Architecture
### 📀 Memory management
- Lifetime of an obj is determined by ```shared_ptr```/```week_ptr```
- Raw pointers can be used in 3rd party lib api calls
- ```Engine``` is the owner of ```Managers```
- ```Managers``` are the owners of ```Assets``` and ```Entities```
- ```Entity``` is owner of ```Component```
- All objects are exposed by ```week_ptr```. Only owner has ```shared_ptr```
- The destruction flow:
  - ```~Engine``` -> ```~Manager``` -> ```~Asset``` / ```~Entity``` -> ```~Component```

### 🔦 Rendering
- Only 2d support
- Call order: ```Engine``` -> ```RenderPipeline``` -> ```RenderComponent``` -> Binding ```Mesh```/```Material```/```Asset```
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
    - ```UiLayouts``` alter the ```Model``` matrix. It is for screen responsive
  - 3d World:
    - Model-View-Projection
    - ```RenderPipline``` orders by Z only ```MeshRenderers```
### 🗂️ Asset management
- Yaml serialization
- File name agnostic, generates .meta files for reference tracking
  🏃

### 🧠 What is missing
- Proper editor. Used AI generated scripts in python, please read that code before using it.
- Proper FSM for App-State management and animation state behaviour
- Sound

## 🏃🏃🏃 TODO 🏃🏃🏃
- [ ] Shooter behaviour
- [ ] PathFinder A*
- [ ] One enemy that tries to kill + spawners
- [ ] Light2d better ray-detection
- [ ] UI buttons
- [ ] One level
- [ ] Character Controller balancing
- [ ] Webgl build
- [ ] Windows build

🇲🇩