#pragma once

#include "system/input_system.hpp"
#include "system/window.hpp"
#include "asset/loaders/asset_loader.hpp"
#include "project/project_asset_loader.hpp"
#include <thread>


#include "system/time.hpp"
#include "asset/asset_manager.hpp"
#include "fsm/fsm_controller.hpp"
#include "renderer/ui_manager.hpp"
#include "renderer/controller/opengl_render_controller.hpp"
#include "renderer/video/video_recorder.hpp"
#include "scene/scene_manager.hpp"

class Engine {

private:
    std::shared_ptr<InputSystem> _inputSystem;
    std::shared_ptr<AssetManager> _assetManager;
    std::shared_ptr<SceneManager> _sceneManager;
    std::shared_ptr<UIManager> _uiManager;
    std::shared_ptr<Window> _window;
    std::shared_ptr<OpenGLRenderController> _renderController;
    std::shared_ptr<VideoRecorder> _videoRecorder;

    std::filesystem::path _projectPath;
    ProjectAsset _projectAsset;
    std::unique_ptr<FsmController> _fsmController;

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

    [[nodiscard]] std::shared_ptr<VideoRecorder> GetVideoRecorder() const { return _videoRecorder; }
};