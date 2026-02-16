#pragma once
#include "../fsm/node/action/action.hpp"
#include "guid.hpp"
#include "audio_manager.hpp"
#include "../esc/animation/fsm_animation_component.hpp"
#include <memory>
#include <string>
#include <chrono>

struct PlaySoundRepeatedAction final : public Action {
private:
    Guid _audioGuid;
    float _volume;
    float _delaySeconds;
    bool _spatial;
    float _minDistance;
    float _maxDistance;
    std::weak_ptr<AudioManager> _audioManager;
    std::weak_ptr<FsmAnimationComponent> _animationComponent;
    mutable std::chrono::steady_clock::time_point _lastPlayTime;
    mutable bool _firstPlay = true;

public:
    PlaySoundRepeatedAction(Guid audioGuid, float volume, float delaySeconds,
                            bool spatial, float minDistance, float maxDistance,
                            std::weak_ptr<AudioManager> audioManager,
                            std::weak_ptr<FsmAnimationComponent> animationComponent = {})
        : _audioGuid(std::move(audioGuid)),
          _volume(volume),
          _delaySeconds(delaySeconds),
          _spatial(spatial),
          _minDistance(minDistance),
          _maxDistance(maxDistance),
          _audioManager(std::move(audioManager)),
          _animationComponent(std::move(animationComponent)) {}

    void OnEnter() const override {
        _firstPlay = true;
        _lastPlayTime = std::chrono::steady_clock::now();
    }

    void OnUpdate() const override {
        if (_audioGuid.IsEmpty()) return;

        const auto now = std::chrono::steady_clock::now();
        const float elapsed = std::chrono::duration<float>(now - _lastPlayTime).count();

        if (_firstPlay || elapsed >= _delaySeconds) {
            if (const auto manager = _audioManager.lock()) {
                if (_spatial) {
                    glm::vec3 position(0.0f);
                    if (const auto anim = _animationComponent.lock()) {
                        position = anim->EntityPosition;
                    }
                    manager->PlaySoundSpatial(_audioGuid, position, _volume, false, _minDistance, _maxDistance);
                } else {
                    manager->PlaySound(_audioGuid, _volume, false);
                }
            }
            _lastPlayTime = now;
            _firstPlay = false;
        }
    }

    void OnExit() const override {
        _firstPlay = true;
    }
};
