#include "framebuffer.hpp"

Framebuffer::~Framebuffer() {
    if (_fbo != 0) {
        glDeleteFramebuffers(1, &_fbo);
    }
    if (_colorTexture != 0) {
        glDeleteTextures(1, &_colorTexture);
    }
    if (_depthTexture != 0) {
        glDeleteTextures(1, &_depthTexture);
    }
}

void Framebuffer::Init(uint16_t width, uint16_t height) {
    if (_initialized) {
        Resize(width, height);
        return;
    }

    _width = width;
    _height = height;

    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glGenTextures(1, &_colorTexture);
    glBindTexture(GL_TEXTURE_2D, _colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTexture, 0);

    glGenTextures(1, &_depthTexture);
    glBindTexture(GL_TEXTURE_2D, _depthTexture);
#ifdef __EMSCRIPTEN__
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _width, _height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, nullptr);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, _width, _height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifdef __EMSCRIPTEN__
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexture, 0);
#else
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _depthTexture, 0);
#endif

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _initialized = true;
}

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glViewport(0, 0, _width, _height);
}

void Framebuffer::Unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::BindColorTexture(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, _colorTexture);
}

void Framebuffer::BindDepthTexture(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, _depthTexture);
}

void Framebuffer::Resize(uint16_t width, uint16_t height) {
    if (_width == width && _height == height) {
        return;
    }

    _width = width;
    _height = height;

    glBindTexture(GL_TEXTURE_2D, _colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, _depthTexture);
#ifdef __EMSCRIPTEN__
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _width, _height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, nullptr);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, _width, _height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
#endif
}
