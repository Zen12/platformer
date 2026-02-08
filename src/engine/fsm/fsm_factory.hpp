#pragma once

#include <memory>
#include "fsm_controller.hpp"
#include "fsm_builder.hpp"
#include "fsm_asset.hpp"
#include "trigger_board.hpp"

class FsmAnimationComponent;
class VideoRecorder;
class AudioManager;
class UIManager;
class SceneManager;

class FsmFactory final {
public:
    static std::shared_ptr<FsmController> Create(
        const FsmAsset& fsmAsset,
        const std::shared_ptr<SceneManager>& sceneManager,
        const std::shared_ptr<UIManager>& uiManager,
        const std::shared_ptr<VideoRecorder>& videoRecorder = nullptr,
        const std::shared_ptr<AudioManager>& audioManager = nullptr,
        const std::weak_ptr<FsmAnimationComponent>& animationComponent = std::weak_ptr<FsmAnimationComponent>()
    ) {
        auto triggerBoard = std::make_shared<TriggerBoard>();

        FsmData data = FsmBuilder::Build(
            fsmAsset,
            triggerBoard,
            uiManager,
            sceneManager,
            videoRecorder,
            audioManager,
            animationComponent
        );

        return std::make_shared<FsmController>(std::move(data), triggerBoard);
    }
};
