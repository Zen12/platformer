#pragma once

#ifndef __EMSCRIPTEN__

#include <map>
#include <memory>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "font.hpp"
#include "terminal_emulator.hpp"

class Shader;

class TerminalRenderer {
public:
    TerminalRenderer() = default;
    ~TerminalRenderer();

    TerminalRenderer(const TerminalRenderer&) = delete;
    TerminalRenderer& operator=(const TerminalRenderer&) = delete;

    void Init(const std::string& fontPath, int fontSize = 16);
    void Render(const TerminalEmulator& emulator, int windowWidth, int windowHeight);
    void Cleanup();

    [[nodiscard]] int GetCellWidth() const noexcept { return _cellWidth; }
    [[nodiscard]] int GetCellHeight() const noexcept { return _cellHeight; }

private:
    void LoadFont(const std::string& fontPath, int fontSize);
    void InitGL();
    const Character* GetGlyph(uint32_t codepoint);
    void RenderBackgrounds(const TerminalEmulator& emulator, float originX, float originY, const glm::mat4& projection);
    void RenderGlyphs(const TerminalEmulator& emulator, float originX, float originY, const glm::mat4& projection);
    void RenderCursor(const TerminalEmulator& emulator, float originX, float originY, const glm::mat4& projection);

    std::unique_ptr<Shader> _glyphShader;
    std::unique_ptr<Shader> _bgShader;
    GLuint _vao = 0;
    GLuint _vbo = 0;

    std::map<uint32_t, Character> _characters;
    int _cellWidth = 0;
    int _cellHeight = 0;
    int _ascender = 0;
    bool _initialized = false;
    int _frameCount = 0;

    FT_Library _ftLib = nullptr;
    FT_Face _ftFace = nullptr;
};

#endif
