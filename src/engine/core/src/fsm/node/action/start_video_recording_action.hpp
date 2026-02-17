#pragma once
#include "action.hpp"
#include "video/video_recorder.hpp"
#include <memory>
#include <string>

struct StartVideoRecordingAction final : public Action {
private:
    std::string _outputPath;
    int _fps;
    std::shared_ptr<VideoRecorder> _videoRecorder;

public:
    StartVideoRecordingAction(std::string outputPath, int fps, std::shared_ptr<VideoRecorder> videoRecorder)
        : _outputPath(std::move(outputPath)),
          _fps(fps),
          _videoRecorder(std::move(videoRecorder)) {}

    void OnEnter() const override {
        if (_videoRecorder) {
            _videoRecorder->StartRecording(_outputPath, _fps);
        }
    }

    void OnUpdate() const override {
        // Recording happens automatically in the render loop
    }

    void OnExit() const override {
        // Recording continues until explicitly stopped via StopVideoRecordingAction
    }
};
