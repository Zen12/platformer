#pragma once
#include <GL/glew.h>
#include <cstdint>

class Framebuffer {
    GLuint _fbo{0};
    GLuint _colorTexture{0};
    GLuint _depthRenderbuffer{0};
    uint16_t _width{0};
    uint16_t _height{0};
    bool _initialized{false};

public:
    Framebuffer() = default;
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;

    void Init(uint16_t width, uint16_t height);
    void Bind() const;
    void Unbind() const;
    void BindColorTexture(GLuint unit) const;
    void Resize(uint16_t width, uint16_t height);

    [[nodiscard]] uint16_t GetWidth() const noexcept { return _width; }
    [[nodiscard]] uint16_t GetHeight() const noexcept { return _height; }
    [[nodiscard]] bool IsInitialized() const noexcept { return _initialized; }
};
