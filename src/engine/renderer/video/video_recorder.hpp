#pragma once

#ifdef VIDEO_RECORDING
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
#endif

#include <string>
#include <vector>
#include <memory>
#include <chrono>

class VideoRecorder {
private:
#ifdef VIDEO_RECORDING
    // FFmpeg contexts
    AVFormatContext* _formatContext = nullptr;
    AVCodecContext* _codecContext = nullptr;
    AVStream* _stream = nullptr;
    SwsContext* _swsContext = nullptr;
    AVFrame* _frame = nullptr;
    AVPacket* _packet = nullptr;

    // Recording state
    bool _isRecording = false;
    int _width = 0;
    int _height = 0;
    int _fps = 60;
    int _frameCount = 0;
    std::string _outputPath;
    std::chrono::steady_clock::time_point _startTime;

    // Frame buffer for OpenGL data
    std::vector<uint8_t> _frameBuffer;

    // Private helper methods
    bool InitializeEncoder();
    void CleanupEncoder();
    bool WriteFrame(AVFrame* frame);
    void FlushEncoder();
#else
    // Stub state when video recording is disabled
    bool _isRecording = false;
    int _frameCount = 0;
#endif

public:
    VideoRecorder() = default;
    ~VideoRecorder();

    // Prevent copying
    VideoRecorder(const VideoRecorder&) = delete;
    VideoRecorder& operator=(const VideoRecorder&) = delete;

    // Start recording
    bool StartRecording(const std::string& outputPath, int fps = 60);

    // Capture current OpenGL frame
    void CaptureFrame();

    // Stop recording and finalize video file
    void StopRecording();

    // Check if currently recording
    [[nodiscard]] bool IsRecording() const noexcept { return _isRecording; }

    // Get current frame count
    [[nodiscard]] int GetFrameCount() const noexcept { return _frameCount; }
};
