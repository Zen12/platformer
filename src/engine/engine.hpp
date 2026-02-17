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
#include "resource_cache.hpp"

class Engine {

private:
    std::shared_ptr<InputSystem> _inputSystem;
    std::shared_ptr<AssetManager> _assetManager;
    std::shared_ptr<ResourceCache> _resourceCache;
    std::shared_ptr<SceneManager> _sceneManager;
    std::shared_ptr<UIManager> _uiManager;
    std::shared_ptr<Window> _window;
    std::shared_ptr<OpenGLRenderController> _renderController;
    std::shared_ptr<VideoRecorder> _videoRecorder;
    std::shared_ptr<AudioManager> _audioManager;

    std::filesystem::path _projectPath;
    ProjectAsset _projectAsset;
    std::shared_ptr<FsmController> _fsmController;

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
    [[nodiscard]] std::shared_ptr<AudioManager> GetAudioManager() const { return _audioManager; }
};