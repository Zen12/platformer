#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <glm/vec3.hpp>

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include "audio_data.hpp"
#include "audio_loader.hpp"
#include "../asset/asset_manager.hpp"

using AudioHandle = uint32_t;
constexpr AudioHandle INVALID_AUDIO_HANDLE = 0;

class AudioManager : public std::enable_shared_from_this<AudioManager> {
private:
    ALCdevice* _device = nullptr;
    ALCcontext* _context = nullptr;

    std::weak_ptr<AssetManager> _assetManager;

    std::unordered_map<std::string, ALuint> _bufferCache;
    std::vector<ALuint> _activeSources;
    ALuint _nextHandle = 1;

    static constexpr int MAX_SOURCES = 32;

public:
    explicit AudioManager(const std::weak_ptr<AssetManager>& assetManager)
        : _assetManager(assetManager) {}

    ~AudioManager() {
        Shutdown();
    }

    bool Initialize() {
        _device = alcOpenDevice(nullptr);
        if (!_device) {
            std::cerr << "AudioManager: Failed to open OpenAL device" << std::endl;
            return false;
        }

        _context = alcCreateContext(_device, nullptr);
        if (!_context) {
            std::cerr << "AudioManager: Failed to create OpenAL context" << std::endl;
            alcCloseDevice(_device);
            _device = nullptr;
            return false;
        }

        if (!alcMakeContextCurrent(_context)) {
            std::cerr << "AudioManager: Failed to make OpenAL context current" << std::endl;
            alcDestroyContext(_context);
            alcCloseDevice(_device);
            _context = nullptr;
            _device = nullptr;
            return false;
        }

        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
        alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        ALfloat orientation[] = {0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f};
        alListenerfv(AL_ORIENTATION, orientation);

        std::cout << "AudioManager: OpenAL initialized successfully" << std::endl;
        return true;
    }

    void Shutdown() {
        StopAllSounds();

        for (auto& [guid, buffer] : _bufferCache) {
            alDeleteBuffers(1, &buffer);
        }
        _bufferCache.clear();

        if (_context) {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(_context);
            _context = nullptr;
        }

        if (_device) {
            alcCloseDevice(_device);
            _device = nullptr;
        }
    }

    AudioHandle PlaySound(const std::string& guid, float volume = 1.0f, bool loop = false) {
        const ALuint buffer = GetOrLoadBuffer(guid);
        if (buffer == 0) {
            return INVALID_AUDIO_HANDLE;
        }

        ALuint source;
        alGenSources(1, &source);

        if (alGetError() != AL_NO_ERROR) {
            std::cerr << "AudioManager: Failed to generate source" << std::endl;
            return INVALID_AUDIO_HANDLE;
        }

        alSourcei(source, AL_BUFFER, static_cast<ALint>(buffer));
        alSourcef(source, AL_GAIN, volume);
        alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
        alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
        alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

        alSourcePlay(source);
        _activeSources.push_back(source);

        return source;
    }

    AudioHandle PlaySoundSpatial(const std::string& guid, const glm::vec3& position,
                                  float volume = 1.0f, bool loop = false,
                                  float minDistance = 1.0f, float maxDistance = 100.0f) {
        const ALuint buffer = GetOrLoadBuffer(guid);
        if (buffer == 0) {
            return INVALID_AUDIO_HANDLE;
        }

        ALuint source;
        alGenSources(1, &source);

        if (alGetError() != AL_NO_ERROR) {
            std::cerr << "AudioManager: Failed to generate spatial source" << std::endl;
            return INVALID_AUDIO_HANDLE;
        }

        alSourcei(source, AL_BUFFER, static_cast<ALint>(buffer));
        alSourcef(source, AL_GAIN, volume);
        alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
        alSource3f(source, AL_POSITION, position.x, position.y, position.z);
        alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
        alSourcef(source, AL_REFERENCE_DISTANCE, minDistance);
        alSourcef(source, AL_MAX_DISTANCE, maxDistance);
        alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);

        alSourcePlay(source);
        _activeSources.push_back(source);

        return source;
    }

    void StopSound(AudioHandle handle) {
        if (handle == INVALID_AUDIO_HANDLE) return;

        const auto source = static_cast<ALuint>(handle);
        alSourceStop(source);
        alDeleteSources(1, &source);

        auto it = std::find(_activeSources.begin(), _activeSources.end(), source);
        if (it != _activeSources.end()) {
            _activeSources.erase(it);
        }
    }

    void StopAllSounds() {
        for (const ALuint source : _activeSources) {
            alSourceStop(source);
            alDeleteSources(1, &source);
        }
        _activeSources.clear();
    }

    void SetListenerPosition(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up) {
        alListener3f(AL_POSITION, position.x, position.y, position.z);
        ALfloat orientation[] = {forward.x, forward.y, forward.z, up.x, up.y, up.z};
        alListenerfv(AL_ORIENTATION, orientation);
    }

    void SetSourcePosition(AudioHandle handle, const glm::vec3& position) {
        if (handle == INVALID_AUDIO_HANDLE) return;
        alSource3f(static_cast<ALuint>(handle), AL_POSITION, position.x, position.y, position.z);
    }

    void SetSourceVolume(AudioHandle handle, float volume) {
        if (handle == INVALID_AUDIO_HANDLE) return;
        alSourcef(static_cast<ALuint>(handle), AL_GAIN, volume);
    }

    [[nodiscard]] bool IsPlaying(AudioHandle handle) const {
        if (handle == INVALID_AUDIO_HANDLE) return false;
        ALint state;
        alGetSourcei(static_cast<ALuint>(handle), AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }

    void Update() {
        std::vector<ALuint> finishedSources;

        for (const ALuint source : _activeSources) {
            ALint state;
            alGetSourcei(source, AL_SOURCE_STATE, &state);
            if (state == AL_STOPPED) {
                finishedSources.push_back(source);
            }
        }

        for (const ALuint source : finishedSources) {
            alDeleteSources(1, &source);
            auto it = std::find(_activeSources.begin(), _activeSources.end(), source);
            if (it != _activeSources.end()) {
                _activeSources.erase(it);
            }
        }
    }

    [[nodiscard]] bool IsInitialized() const noexcept {
        return _device != nullptr && _context != nullptr;
    }

private:
    ALuint GetOrLoadBuffer(const std::string& guid) {
        auto it = _bufferCache.find(guid);
        if (it != _bufferCache.end()) {
            return it->second;
        }

        const auto assetManager = _assetManager.lock();
        if (!assetManager) {
            std::cerr << "AudioManager: AssetManager not available" << std::endl;
            return 0;
        }

        const std::string path = assetManager->GetPathFromGuid(guid);
        const AudioData audioData = AudioLoader::LoadWAV(path);

        if (!audioData.IsValid()) {
            std::cerr << "AudioManager: Failed to load audio: " << guid << std::endl;
            return 0;
        }

        ALuint buffer;
        alGenBuffers(1, &buffer);

        ALenum format = AL_FORMAT_MONO16;
        if (audioData.Channels == 1) {
            format = (audioData.BitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
        } else if (audioData.Channels == 2) {
            format = (audioData.BitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
        }

        alBufferData(buffer, format, audioData.Data.data(),
                     static_cast<ALsizei>(audioData.Data.size()),
                     static_cast<ALsizei>(audioData.SampleRate));

        if (alGetError() != AL_NO_ERROR) {
            std::cerr << "AudioManager: Failed to buffer audio data: " << guid << std::endl;
            alDeleteBuffers(1, &buffer);
            return 0;
        }

        _bufferCache[guid] = buffer;
        return buffer;
    }
};
