#pragma once
#include "../fsm/node/action/action.hpp"
#include "audio_manager.hpp"
#include <memory>
#include <string>

struct PlaySoundAction final : public Action {
private:
    std::string _audioGuid;
    float _volume;
    bool _loop;
    std::weak_ptr<AudioManager> _audioManager;
    mutable AudioHandle _handle = INVALID_AUDIO_HANDLE;

public:
    PlaySoundAction(std::string audioGuid, float volume, bool loop,
                    std::weak_ptr<AudioManager> audioManager)
        : _audioGuid(std::move(audioGuid)),
          _volume(volume),
          _loop(loop),
          _audioManager(std::move(audioManager)) {}

    void OnEnter() const override {
        if (_audioGuid.empty()) return;

        if (const auto manager = _audioManager.lock()) {
            _handle = manager->PlaySound(_audioGuid, _volume, _loop);
        }
    }

    void OnUpdate() const override {
        // Nothing to update
    }

    void OnExit() const override {
        // Stop looping sounds when exiting the state
        if (_loop && _handle != INVALID_AUDIO_HANDLE) {
            if (const auto manager = _audioManager.lock()) {
                manager->StopSound(_handle);
            }
        }
        _handle = INVALID_AUDIO_HANDLE;
    }
};
