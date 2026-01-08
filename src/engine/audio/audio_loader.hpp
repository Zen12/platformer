#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include "audio_data.hpp"

class AudioLoader {
public:
    static AudioData LoadWAV(const std::string& path) {
        AudioData audio;

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "AudioLoader: Failed to open file: " << path << std::endl;
            return audio;
        }

        char chunkId[4];
        uint32_t chunkSize;
        char format[4];

        file.read(chunkId, 4);
        if (std::string(chunkId, 4) != "RIFF") {
            std::cerr << "AudioLoader: Invalid WAV file (missing RIFF): " << path << std::endl;
            return audio;
        }

        file.read(reinterpret_cast<char*>(&chunkSize), 4);
        file.read(format, 4);

        if (std::string(format, 4) != "WAVE") {
            std::cerr << "AudioLoader: Invalid WAV file (missing WAVE): " << path << std::endl;
            return audio;
        }

        char subchunk1Id[4];
        uint32_t subchunk1Size;
        uint16_t audioFormat;
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;

        file.read(subchunk1Id, 4);
        file.read(reinterpret_cast<char*>(&subchunk1Size), 4);

        if (std::string(subchunk1Id, 4) != "fmt ") {
            std::cerr << "AudioLoader: Invalid WAV file (missing fmt): " << path << std::endl;
            return audio;
        }

        file.read(reinterpret_cast<char*>(&audioFormat), 2);
        file.read(reinterpret_cast<char*>(&numChannels), 2);
        file.read(reinterpret_cast<char*>(&sampleRate), 4);
        file.read(reinterpret_cast<char*>(&byteRate), 4);
        file.read(reinterpret_cast<char*>(&blockAlign), 2);
        file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

        if (audioFormat != 1) {
            std::cerr << "AudioLoader: Only PCM format is supported: " << path << std::endl;
            return audio;
        }

        if (subchunk1Size > 16) {
            file.seekg(subchunk1Size - 16, std::ios::cur);
        }

        char subchunk2Id[4];
        uint32_t subchunk2Size;

        while (file.read(subchunk2Id, 4)) {
            file.read(reinterpret_cast<char*>(&subchunk2Size), 4);

            if (std::string(subchunk2Id, 4) == "data") {
                break;
            }
            file.seekg(subchunk2Size, std::ios::cur);
        }

        if (std::string(subchunk2Id, 4) != "data") {
            std::cerr << "AudioLoader: Invalid WAV file (missing data): " << path << std::endl;
            return audio;
        }

        audio.Data.resize(subchunk2Size);
        file.read(reinterpret_cast<char*>(audio.Data.data()), subchunk2Size);

        audio.SampleRate = sampleRate;
        audio.Channels = numChannels;
        audio.BitsPerSample = bitsPerSample;

        return audio;
    }
};
