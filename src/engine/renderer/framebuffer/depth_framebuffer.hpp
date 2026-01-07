#pragma once
#include <GL/glew.h>
#include <cstdint>

class DepthFramebuffer {
    GLuint _fbo{0};
    GLuint _depthTexture{0};
    uint16_t _width{0};
    uint16_t _height{0};
    bool _initialized{false};

public:
    DepthFramebuffer() = default;
    ~DepthFramebuffer();

    DepthFramebuffer(const DepthFramebuffer&) = delete;
    DepthFramebuffer& operator=(const DepthFramebuffer&) = delete;
    DepthFramebuffer(DepthFramebuffer&&) = delete;
    DepthFramebuffer& operator=(DepthFramebuffer&&) = delete;

    void Init(uint16_t width, uint16_t height);
    void Bind() const;
    void Unbind() const;
    void BindDepthTexture(GLuint unit) const;
    void Resize(uint16_t width, uint16_t height);

    [[nodiscard]] uint16_t GetWidth() const noexcept { return _width; }
    [[nodiscard]] uint16_t GetHeight() const noexcept { return _height; }
    [[nodiscard]] bool IsInitialized() const noexcept { return _initialized; }
};
