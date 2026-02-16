#pragma once
#include "action.hpp"
#include "video/video_recorder.hpp"
#include <memory>

struct StopVideoRecordingAction final : public Action {
private:
    std::shared_ptr<VideoRecorder> _videoRecorder;

public:
    explicit StopVideoRecordingAction(std::shared_ptr<VideoRecorder> videoRecorder)
        : _videoRecorder(std::move(videoRecorder)) {}

    void OnEnter() const override {
        if (_videoRecorder) {
            _videoRecorder->StopRecording();
        }
    }

    void OnUpdate() const override {
        // Nothing to do
    }

    void OnExit() const override {
        // Nothing to do
    }
};
