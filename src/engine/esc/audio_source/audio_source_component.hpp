#pragma once
#include "../../audio/audio_manager.hpp"
#include "../../system/guid.hpp"

class AudioSourceComponent final {
private:
    Guid _audioClipGuid;
    float _volume = 1.0f;
    float _pitch = 1.0f;
    bool _loop = false;
    bool _autoPlay = false;
    bool _spatial = true;
    float _minDistance = 1.0f;
    float _maxDistance = 100.0f;

    AudioHandle _handle = INVALID_AUDIO_HANDLE;
    bool _hasStarted = false;

public:
    AudioSourceComponent() = default;

    AudioSourceComponent(const Guid& audioClipGuid, float volume, float pitch,
                         bool loop, bool autoPlay, bool spatial,
                         float minDistance, float maxDistance)
        : _audioClipGuid(audioClipGuid), _volume(volume), _pitch(pitch),
          _loop(loop), _autoPlay(autoPlay), _spatial(spatial),
          _minDistance(minDistance), _maxDistance(maxDistance) {}

    [[nodiscard]] const Guid& GetAudioClipGuid() const noexcept { return _audioClipGuid; }
    void SetAudioClipGuid(const Guid& guid) noexcept { _audioClipGuid = guid; }

    [[nodiscard]] float GetVolume() const noexcept { return _volume; }
    void SetVolume(float volume) noexcept { _volume = volume; }

    [[nodiscard]] float GetPitch() const noexcept { return _pitch; }
    void SetPitch(float pitch) noexcept { _pitch = pitch; }

    [[nodiscard]] bool IsLooping() const noexcept { return _loop; }
    void SetLoop(bool loop) noexcept { _loop = loop; }

    [[nodiscard]] bool IsAutoPlay() const noexcept { return _autoPlay; }
    void SetAutoPlay(bool autoPlay) noexcept { _autoPlay = autoPlay; }

    [[nodiscard]] bool IsSpatial() const noexcept { return _spatial; }
    void SetSpatial(bool spatial) noexcept { _spatial = spatial; }

    [[nodiscard]] float GetMinDistance() const noexcept { return _minDistance; }
    void SetMinDistance(float dist) noexcept { _minDistance = dist; }

    [[nodiscard]] float GetMaxDistance() const noexcept { return _maxDistance; }
    void SetMaxDistance(float dist) noexcept { _maxDistance = dist; }

    [[nodiscard]] AudioHandle GetHandle() const noexcept { return _handle; }
    void SetHandle(AudioHandle handle) noexcept { _handle = handle; }

    [[nodiscard]] bool HasStarted() const noexcept { return _hasStarted; }
    void SetHasStarted(bool started) noexcept { _hasStarted = started; }
};
