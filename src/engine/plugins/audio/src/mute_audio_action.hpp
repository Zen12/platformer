#pragma once
#include "node/action/action.hpp"
#include "audio_manager.hpp"
#include <memory>

struct MuteAudioAction final : public Action {
private:
    bool _mute;
    bool _onlyDebug;
    std::weak_ptr<AudioManager> _audioManager;

public:
    MuteAudioAction(bool mute, bool onlyDebug, std::weak_ptr<AudioManager> audioManager)
        : _mute(mute),
          _onlyDebug(onlyDebug),
          _audioManager(std::move(audioManager)) {}

    void OnEnter() const override {
        if (_onlyDebug) {
#ifndef NDEBUG
            return;
#endif
        }

        if (const auto manager = _audioManager.lock()) {
            manager->SetMuted(_mute);
        }
    }

    void OnUpdate() const override {}

    void OnExit() const override {}
};
