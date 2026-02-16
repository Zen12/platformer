#include "video_recorder.hpp"
#include <iostream>

#ifdef VIDEO_RECORDING
#include <GL/glew.h>

VideoRecorder::~VideoRecorder() {
    if (_isRecording) {
        StopRecording();
    }
}

bool VideoRecorder::StartRecording(const std::string& outputPath, int fps) {
    if (_isRecording) {
        std::cerr << "Already recording!" << std::endl;
        return false;
    }

    // Get viewport dimensions from OpenGL
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int viewportWidth = viewport[2];
    int viewportHeight = viewport[3];

    _outputPath = outputPath;
    _width = viewportWidth;   // Use actual viewport width
    _height = viewportHeight; // Use actual viewport height
    _fps = fps;
    _frameCount = 0;
    _startTime = std::chrono::steady_clock::now();

    // Allocate frame buffer for OpenGL readback
    _frameBuffer.resize(_width * _height * 3);  // RGB

    if (!InitializeEncoder()) {
        CleanupEncoder();
        return false;
    }

    _isRecording = true;
    std::cout << "Started recording to: " << _outputPath << " (" << _width << "x" << _height << " @ " << _fps << " fps)" << std::endl;
    return true;
}

bool VideoRecorder::InitializeEncoder() {
    int ret;

    // Allocate format context
    avformat_alloc_output_context2(&_formatContext, nullptr, nullptr, _outputPath.c_str());
    if (!_formatContext) {
        std::cerr << "Could not create output context" << std::endl;
        return false;
    }

    // Find H.264 encoder
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        std::cerr << "H.264 codec not found" << std::endl;
        return false;
    }

    // Create stream
    _stream = avformat_new_stream(_formatContext, nullptr);
    if (!_stream) {
        std::cerr << "Could not create stream" << std::endl;
        return false;
    }
    _stream->id = _formatContext->nb_streams - 1;

    // Allocate codec context
    _codecContext = avcodec_alloc_context3(codec);
    if (!_codecContext) {
        std::cerr << "Could not allocate codec context" << std::endl;
        return false;
    }

    // Set codec parameters
    _codecContext->width = _width;
    _codecContext->height = _height;
    _codecContext->time_base = AVRational{1, _fps};
    _codecContext->framerate = AVRational{_fps, 1};
    _codecContext->gop_size = 12;  // One keyframe every 12 frames
    _codecContext->max_b_frames = 2;
    _codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    _codecContext->bit_rate = 4000000;  // 4 Mbps

    // Set codec options for better quality
    av_opt_set(_codecContext->priv_data, "preset", "medium", 0);
    av_opt_set(_codecContext->priv_data, "crf", "23", 0);  // Quality (lower = better, 23 is default)

    // Some formats require global headers
    if (_formatContext->oformat->flags & AVFMT_GLOBALHEADER) {
        _codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    // Open codec
    ret = avcodec_open2(_codecContext, codec, nullptr);
    if (ret < 0) {
        char errbuf[128];
        av_strerror(ret, errbuf, sizeof(errbuf));
        std::cerr << "Could not open codec: " << errbuf << std::endl;
        return false;
    }

    // Allocate frame
    _frame = av_frame_alloc();
    if (!_frame) {
        std::cerr << "Could not allocate frame" << std::endl;
        return false;
    }
    _frame->format = _codecContext->pix_fmt;
    _frame->width = _codecContext->width;
    _frame->height = _codecContext->height;

    ret = av_frame_get_buffer(_frame, 0);
    if (ret < 0) {
        std::cerr << "Could not allocate frame data" << std::endl;
        return false;
    }

    // Copy codec parameters to stream
    ret = avcodec_parameters_from_context(_stream->codecpar, _codecContext);
    if (ret < 0) {
        std::cerr << "Could not copy codec parameters" << std::endl;
        return false;
    }

    _stream->time_base = _codecContext->time_base;

    // Open output file
    if (!(_formatContext->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&_formatContext->pb, _outputPath.c_str(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            char errbuf[128];
            av_strerror(ret, errbuf, sizeof(errbuf));
            std::cerr << "Could not open output file: " << errbuf << std::endl;
            return false;
        }
    }

    // Write header
    ret = avformat_write_header(_formatContext, nullptr);
    if (ret < 0) {
        char errbuf[128];
        av_strerror(ret, errbuf, sizeof(errbuf));
        std::cerr << "Could not write header: " << errbuf << std::endl;
        return false;
    }

    // Allocate packet
    _packet = av_packet_alloc();
    if (!_packet) {
        std::cerr << "Could not allocate packet" << std::endl;
        return false;
    }

    // Initialize software scaler for RGB -> YUV conversion
    _swsContext = sws_getContext(
        _width, _height, AV_PIX_FMT_RGB24,
        _width, _height, AV_PIX_FMT_YUV420P,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );
    if (!_swsContext) {
        std::cerr << "Could not initialize sws context" << std::endl;
        return false;
    }

    return true;
}

void VideoRecorder::CaptureFrame() {
    if (!_isRecording) {
        return;
    }

    // Read pixels from OpenGL framebuffer (bottom-left origin)
    glReadPixels(0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, _frameBuffer.data());

    // Prepare source frame for conversion (RGB, needs to be flipped vertically)
    AVFrame* rgbFrame = av_frame_alloc();
    rgbFrame->format = AV_PIX_FMT_RGB24;
    rgbFrame->width = _width;
    rgbFrame->height = _height;
    av_frame_get_buffer(rgbFrame, 0);

    // Copy and flip vertically (OpenGL has origin at bottom-left, video at top-left)
    for (int y = 0; y < _height; y++) {
        memcpy(
            rgbFrame->data[0] + y * rgbFrame->linesize[0],
            _frameBuffer.data() + (_height - 1 - y) * _width * 3,
            _width * 3
        );
    }

    // Make frame writable
    int ret = av_frame_make_writable(_frame);
    if (ret < 0) {
        std::cerr << "Could not make frame writable" << std::endl;
        av_frame_free(&rgbFrame);
        return;
    }

    // Convert RGB to YUV
    sws_scale(
        _swsContext,
        rgbFrame->data,
        rgbFrame->linesize,
        0,
        _height,
        _frame->data,
        _frame->linesize
    );

    av_frame_free(&rgbFrame);

    // Set frame timestamp based on actual elapsed time
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - _startTime);
    double elapsedSeconds = elapsedTime.count() / 1000000.0;

    // Convert to PTS using the video's timebase (1/fps)
    // PTS = elapsed_seconds * fps
    _frame->pts = static_cast<int64_t>(elapsedSeconds * _fps);

    _frameCount++;

    // Encode frame
    WriteFrame(_frame);
}

bool VideoRecorder::WriteFrame(AVFrame* frame) {
    int ret;

    // Send frame to encoder
    ret = avcodec_send_frame(_codecContext, frame);
    if (ret < 0) {
        char errbuf[128];
        av_strerror(ret, errbuf, sizeof(errbuf));
        std::cerr << "Error sending frame to encoder: " << errbuf << std::endl;
        return false;
    }

    // Receive encoded packets
    while (ret >= 0) {
        ret = avcodec_receive_packet(_codecContext, _packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            char errbuf[128];
            av_strerror(ret, errbuf, sizeof(errbuf));
            std::cerr << "Error receiving packet from encoder: " << errbuf << std::endl;
            return false;
        }

        // Rescale packet timestamp
        av_packet_rescale_ts(_packet, _codecContext->time_base, _stream->time_base);
        _packet->stream_index = _stream->index;

        // Write packet to output file
        ret = av_interleaved_write_frame(_formatContext, _packet);
        if (ret < 0) {
            char errbuf[128];
            av_strerror(ret, errbuf, sizeof(errbuf));
            std::cerr << "Error writing packet: " << errbuf << std::endl;
            av_packet_unref(_packet);
            return false;
        }

        av_packet_unref(_packet);
    }

    return true;
}

void VideoRecorder::FlushEncoder() {
    // Send null frame to flush encoder
    avcodec_send_frame(_codecContext, nullptr);

    int ret = 0;
    while (ret >= 0) {
        ret = avcodec_receive_packet(_codecContext, _packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            break;
        }

        av_packet_rescale_ts(_packet, _codecContext->time_base, _stream->time_base);
        _packet->stream_index = _stream->index;

        av_interleaved_write_frame(_formatContext, _packet);
        av_packet_unref(_packet);
    }
}

void VideoRecorder::StopRecording() {
    if (!_isRecording) {
        return;
    }

    // Flush encoder
    FlushEncoder();

    // Write trailer
    av_write_trailer(_formatContext);

    std::cout << "Stopped recording. Wrote " << _frameCount << " frames to " << _outputPath << std::endl;

    CleanupEncoder();
    _isRecording = false;
}

void VideoRecorder::CleanupEncoder() {
    if (_swsContext) {
        sws_freeContext(_swsContext);
        _swsContext = nullptr;
    }

    if (_frame) {
        av_frame_free(&_frame);
    }

    if (_packet) {
        av_packet_free(&_packet);
    }

    if (_codecContext) {
        avcodec_free_context(&_codecContext);
    }

    if (_formatContext) {
        if (!(_formatContext->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&_formatContext->pb);
        }
        avformat_free_context(_formatContext);
        _formatContext = nullptr;
    }
}

#else
// Stub implementations when VIDEO_RECORDING is disabled

VideoRecorder::~VideoRecorder() = default;

bool VideoRecorder::StartRecording([[maybe_unused]] const std::string& outputPath,
                                    [[maybe_unused]] int fps) {
    return false;
}

void VideoRecorder::CaptureFrame() {
    // Do nothing when recording is disabled
}

void VideoRecorder::StopRecording() {
    // Do nothing when recording is disabled
}

#endif
