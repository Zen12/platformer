#pragma once


#include <GL/glew.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <GLFW/glfw3.h>

#include "render/mesh.hpp"
#include "system/input_system.hpp"
#include "system/window.hpp"
#include "asset/asset_loader.hpp"

#include "components/entity.hpp"
#include "components/sprite_renderer.hpp"
#include "components/transforms/transform.hpp"
#include "components/transforms/rect_transform.hpp"
#include "components/transforms/rect_transform_root.hpp"
#include "components/camera_component.hpp"
#include "components/ui_text_renderer.hpp"
#include "components/physics/box_collider2d_component.hpp"
#include "components/physics/rigidbody2d_component.hpp"


#include "render_pipeline/render_pipeline.hpp"
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
    std::shared_ptr<RenderPipeline> _renderPipeline;
    std::shared_ptr<PhysicsWorld> _physicsWorld;

    std::string _projectPath;
    ProjectAsset _projectAsset;

    std::weak_ptr<UiTextRenderer> _fpsText;
    Time _timer;

public:
    explicit Engine(const std::string &projectPath);

    void LoadFirstScene();

    ~Engine();

    void Tick();
    [[nodiscard]] bool IsTickable() const;
};