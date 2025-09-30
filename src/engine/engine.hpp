#pragma once


#include <GL/glew.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <GLFW/glfw3.h>

#include "renderer/mesh/mesh.hpp"
#include "system/input_system.hpp"
#include "system/window.hpp"
#include "asset/loaders/asset_loader.hpp"
#include <thread>

#include "entity/entity.hpp"
#include "renderer/sprite/sprite_renderer_component.hpp"
#include "renderer/transform/transform_component.hpp"
#include "ui/rect_transform/rect_transform_component.hpp"
#include "renderer/transform/rect_transform_root.hpp"
#include "renderer/camera/camera_component.hpp"
#include "ui/text/text_renderer_component.hpp"
#include "physics/collider/box_collider2d_component.hpp"
#include "physics/rigidbody/rigidbody2d_component.hpp"


#include "renderer/render_pipeline.hpp"
#include "system/time.hpp"
#include "asset/asset_manager.hpp"
#include "scene/scene_manager.hpp"

#include "physics/physics_world.hpp"

class Engine {

private:
    std::shared_ptr<InputSystem> _inputSystem;
    std::shared_ptr<AssetManager> _assetManager;
    std::shared_ptr<SceneManager> _sceneManager;
    std::shared_ptr<Window> _window;

    std::filesystem::path _projectPath;
    ProjectAsset _projectAsset;

    Time _frameTimer;

    bool _isReloadRequested = false;
    float _targetFrameTime = 0.02f;

public:
    explicit Engine(const std::filesystem::path &projectPath);

    void LoadFirstScene();

    void WaitForTargetFrameRate() const;

    ~Engine();

    void Tick();
    [[nodiscard]] bool IsTickable() const;

    [[nodiscard]] bool IsReloadRequested() const;
};