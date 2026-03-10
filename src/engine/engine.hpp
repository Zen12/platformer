#pragma once

#include "input_system.hpp"
#include "window.hpp"
#include "loaders/asset_loader.hpp"
#include "project_asset_loader.hpp"
#include <thread>


#include "time.hpp"
#include "asset_manager.hpp"
#include "fsm_controller.hpp"
#include "ui_manager.hpp"
#include "controller/opengl_render_controller.hpp"
#include "video/video_recorder.hpp"
#include "scene_manager.hpp"
#include "audio_manager.hpp"
#include "resource_factory.hpp"
#include "core/src/register/fsm_action_registry.hpp"
#include "core/src/register/fsm_condition_registry.hpp"
#include "engine_context.hpp"
#include "entity/component_registry.hpp"
#include "core/src/register/esc_system_registry.hpp"
#include "core/src/plugin/plugin_registry.hpp"

class Engine {

private:
    std::shared_ptr<InputSystem> _inputSystem;
    std::shared_ptr<AssetManager> _assetManager;
    std::shared_ptr<ResourceFactory> _resourceFactory;
    std::shared_ptr<SceneManager> _sceneManager;
    std::shared_ptr<UIManager> _uiManager;
    std::shared_ptr<Window> _window;
    std::shared_ptr<OpenGLRenderController> _renderController;
    std::shared_ptr<VideoRecorder> _videoRecorder;
    std::shared_ptr<AudioManager> _audioManager;

    std::filesystem::path _projectPath;
    ProjectAsset _projectAsset;
    std::shared_ptr<FsmController> _fsmController;

    FsmActionRegistry _actionRegistry;
    FsmConditionRegistry _conditionRegistry;
    EngineContext _engineContext;
    ComponentRegistry _componentRegistry;
    EscSystemRegistry _systemRegistry;

    Time _frameTimer;

    bool _isReloadRequested = false;
    float _targetFrameTime = 0.02f;

public:
    explicit Engine(const std::filesystem::path &projectPath);

    void WaitForTargetFrameRate() const;

    ~Engine();

    void Tick();
    [[nodiscard]] bool IsTickable() const;

    [[nodiscard]] bool IsReloadRequested() const;
};
