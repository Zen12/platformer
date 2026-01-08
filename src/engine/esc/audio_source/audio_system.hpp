#pragma once
#include "audio_source_component.hpp"
#include "../audio_listener/audio_listener_component.hpp"
#include "../transform/transform_component.hpp"
#include "../esc_core.hpp"
#include "../../audio/audio_manager.hpp"
#include <memory>
#include <cmath>

class AudioSystem final : public ISystemView<AudioSourceComponent, TransformComponentV2> {
private:
    using TypeListener = decltype(std::declval<entt::registry>().view<AudioListenerComponent, TransformComponentV2>());

    const TypeListener _listenerView;
    std::weak_ptr<AudioManager> _audioManager;

public:
    AudioSystem(
        const TypeView& view,
        const TypeListener& listenerView,
        const std::weak_ptr<AudioManager>& audioManager)
        : ISystemView(view), _listenerView(listenerView), _audioManager(audioManager) {}

    void OnTick() override {
        const auto audioManager = _audioManager.lock();
        if (!audioManager || !audioManager->IsInitialized()) {
            return;
        }

        // Update listener position (AudioListenerComponent is a tag, so EnTT only returns entity + transform)
        for (auto [entity, transform] : _listenerView.each()) {
            const glm::vec3 position = transform.GetPosition();
            // Calculate forward from rotation (assuming Y-up, forward is -Z initially)
            const glm::vec3 rotation = transform.GetEulerRotation();
            const float yaw = glm::radians(rotation.y);
            const glm::vec3 forward = glm::vec3(-std::sin(yaw), 0.0f, -std::cos(yaw));
            const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            audioManager->SetListenerPosition(position, forward, up);
            break; // Only one listener
        }

        // Update audio sources
        for (auto [entity, source, transform] : View.each()) {
            // Handle auto-play
            if (source.IsAutoPlay() && !source.HasStarted()) {
                PlaySource(source, transform.GetPosition(), audioManager);
            }

            // Update spatial position if playing
            if (source.IsSpatial() && source.GetHandle() != INVALID_AUDIO_HANDLE) {
                if (audioManager->IsPlaying(source.GetHandle())) {
                    audioManager->SetSourcePosition(source.GetHandle(), transform.GetPosition());
                }
            }
        }
    }

private:
    static void PlaySource(AudioSourceComponent& source, const glm::vec3& position,
                          const std::shared_ptr<AudioManager>& audioManager) {
        if (source.GetAudioClipGuid().empty()) {
            return;
        }

        AudioHandle handle;
        if (source.IsSpatial()) {
            handle = audioManager->PlaySoundSpatial(
                source.GetAudioClipGuid(),
                position,
                source.GetVolume(),
                source.IsLooping(),
                source.GetMinDistance(),
                source.GetMaxDistance()
            );
        } else {
            handle = audioManager->PlaySound(
                source.GetAudioClipGuid(),
                source.GetVolume(),
                source.IsLooping()
            );
        }

        source.SetHandle(handle);
        source.SetHasStarted(true);
    }
};
