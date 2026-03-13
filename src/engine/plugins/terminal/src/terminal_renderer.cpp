#ifndef __EMSCRIPTEN__

#include "terminal_renderer.hpp"
#include "material/shader.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>
#include <vector>

static const char* GLYPH_VERTEX_SHADER = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aColor;
uniform mat4 projection;
out vec2 TexCoord;
out vec3 Color;
void main() {
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
    Color = aColor;
}
)";

static const char* GLYPH_FRAGMENT_SHADER = R"(
#version 330 core
in vec2 TexCoord;
in vec3 Color;
out vec4 FragColor;
uniform sampler2D glyphTexture;
void main() {
    float alpha = texture(glyphTexture, TexCoord).r;
    FragColor = vec4(Color, alpha);
}
)";

static const char* BG_VERTEX_SHADER = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;
uniform mat4 projection;
out vec3 Color;
void main() {
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    Color = aColor;
}
)";

static const char* BG_FRAGMENT_SHADER = R"(
#version 330 core
in vec3 Color;
out vec4 FragColor;
void main() {
    FragColor = vec4(Color, 0.92);
}
)";

TerminalRenderer::~TerminalRenderer() {
    Cleanup();
}

void TerminalRenderer::Init(const std::string& fontPath, int fontSize) {
    if (_initialized) return;
    LoadFont(fontPath, fontSize);
    InitGL();
    _initialized = true;
}

void TerminalRenderer::LoadFont(const std::string& fontPath, int fontSize) {
    if (FT_Init_FreeType(&_ftLib)) {
        std::cerr << "ERROR::TERMINAL: Could not init FreeType" << std::endl;
        return;
    }

    if (FT_New_Face(_ftLib, fontPath.c_str(), 0, &_ftFace)) {
        std::cerr << "ERROR::TERMINAL: Failed to load font: " << fontPath << std::endl;
        FT_Done_FreeType(_ftLib);
        _ftLib = nullptr;
        return;
    }

    FT_Set_Pixel_Sizes(_ftFace, 0, static_cast<FT_UInt>(fontSize));
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (uint32_t c = 0; c < 128; c++) {
        if (FT_Load_Char(_ftFace, c, FT_LOAD_RENDER)) {
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_R8,
            static_cast<int32_t>(_ftFace->glyph->bitmap.width),
            static_cast<int32_t>(_ftFace->glyph->bitmap.rows),
            0, GL_RED, GL_UNSIGNED_BYTE,
            _ftFace->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(_ftFace->glyph->bitmap.width, _ftFace->glyph->bitmap.rows),
            glm::ivec2(_ftFace->glyph->bitmap_left, _ftFace->glyph->bitmap_top),
            static_cast<unsigned int>(_ftFace->glyph->advance.x)
        };
        _characters.insert(std::pair<uint32_t, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    _ascender = static_cast<int>(_ftFace->size->metrics.ascender >> 6);
    _cellHeight = static_cast<int>(_ftFace->size->metrics.height >> 6);

    _cellWidth = 0;
    for (const auto& [ch, character] : _characters) {
        int advance = static_cast<int>(character.Advance >> 6);
        _cellWidth = std::max(_cellWidth, advance);
    }
}

void TerminalRenderer::InitGL() {
    _glyphShader = std::make_unique<Shader>(GLYPH_VERTEX_SHADER, GLYPH_FRAGMENT_SHADER);
    _bgShader = std::make_unique<Shader>(BG_VERTEX_SHADER, BG_FRAGMENT_SHADER);

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
}

const Character* TerminalRenderer::GetGlyph(uint32_t codepoint) {
    auto it = _characters.find(codepoint);
    if (it != _characters.end()) {
        return &it->second;
    }

    if (!_ftFace) return nullptr;

    FT_UInt glyphIndex = FT_Get_Char_Index(_ftFace, codepoint);
    if (glyphIndex == 0) return nullptr;

    if (FT_Load_Glyph(_ftFace, glyphIndex, FT_LOAD_RENDER)) {
        return nullptr;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_R8,
        static_cast<int32_t>(_ftFace->glyph->bitmap.width),
        static_cast<int32_t>(_ftFace->glyph->bitmap.rows),
        0, GL_RED, GL_UNSIGNED_BYTE,
        _ftFace->glyph->bitmap.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    Character character = {
        texture,
        glm::ivec2(_ftFace->glyph->bitmap.width, _ftFace->glyph->bitmap.rows),
        glm::ivec2(_ftFace->glyph->bitmap_left, _ftFace->glyph->bitmap_top),
        static_cast<unsigned int>(_ftFace->glyph->advance.x)
    };
    auto result = _characters.insert(std::pair<uint32_t, Character>(codepoint, character));
    return &result.first->second;
}

void TerminalRenderer::Cleanup() {
    if (_vao) {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }
    if (_vbo) {
        glDeleteBuffers(1, &_vbo);
        _vbo = 0;
    }
    for (auto& [ch, character] : _characters) {
        if (character.TextureID) {
            glDeleteTextures(1, &character.TextureID);
            character.TextureID = 0;
        }
    }
    _characters.clear();
    _glyphShader.reset();
    _bgShader.reset();

    if (_ftFace) {
        FT_Done_Face(_ftFace);
        _ftFace = nullptr;
    }
    if (_ftLib) {
        FT_Done_FreeType(_ftLib);
        _ftLib = nullptr;
    }

    _initialized = false;
}

void TerminalRenderer::Render(const TerminalEmulator& emulator, int windowWidth, int windowHeight) {
    if (!_initialized) return;

    _frameCount++;

    GLboolean depthTestEnabled;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    GLboolean blendEnabled;
    glGetBooleanv(GL_BLEND, &blendEnabled);
    GLint blendSrc, blendDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float fw = static_cast<float>(windowWidth);
    float fh = static_cast<float>(windowHeight);
    glm::mat4 projection = glm::ortho(0.0f, fw, fh, 0.0f);

    float termWidth = static_cast<float>(emulator.GetCols() * _cellWidth);
    float termHeight = static_cast<float>(emulator.GetRows() * _cellHeight);
    float originX = (fw - termWidth) * 0.5f;
    float originY = (fh - termHeight) * 0.5f;

    originX = std::max(0.0f, originX);
    originY = std::max(0.0f, originY);

    RenderBackgrounds(emulator, originX, originY, projection);
    RenderGlyphs(emulator, originX, originY, projection);
    RenderCursor(emulator, originX, originY, projection);

    if (depthTestEnabled) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (!blendEnabled) glDisable(GL_BLEND);
    glBlendFunc(blendSrc, blendDst);
}

void TerminalRenderer::RenderBackgrounds(const TerminalEmulator& emulator, float originX, float originY, const glm::mat4& projection) {
    _bgShader->Use();
    _bgShader->SetMat4(_bgShader->GetLocation("projection"), projection);

    int cols = emulator.GetCols();
    int rows = emulator.GetRows();
    std::vector<float> vertices;
    vertices.reserve(static_cast<size_t>(cols * rows * 6 * 5));

    float cw = static_cast<float>(_cellWidth);
    float ch = static_cast<float>(_cellHeight);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            const auto& cell = emulator.GetCell(row, col);
            float x = originX + static_cast<float>(col) * cw;
            float y = originY + static_cast<float>(row) * ch;

            float r = cell.Bg.r, g = cell.Bg.g, b = cell.Bg.b;

            vertices.insert(vertices.end(), {x,      y,      r, g, b});
            vertices.insert(vertices.end(), {x + cw, y,      r, g, b});
            vertices.insert(vertices.end(), {x + cw, y + ch, r, g, b});
            vertices.insert(vertices.end(), {x,      y,      r, g, b});
            vertices.insert(vertices.end(), {x + cw, y + ch, r, g, b});
            vertices.insert(vertices.end(), {x,      y + ch, r, g, b});
        }
    }

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 5));

    glBindVertexArray(0);
}

void TerminalRenderer::RenderGlyphs(const TerminalEmulator& emulator, float originX, float originY, const glm::mat4& projection) {
    _glyphShader->Use();
    _glyphShader->SetMat4(_glyphShader->GetLocation("projection"), projection);
    _glyphShader->SetInt(_glyphShader->GetLocation("glyphTexture"), 0);

    glActiveTexture(GL_TEXTURE0);

    int cols = emulator.GetCols();
    int rows = emulator.GetRows();
    float cw = static_cast<float>(_cellWidth);
    float ch = static_cast<float>(_cellHeight);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            const auto& cell = emulator.GetCell(row, col);
            if (cell.Codepoint <= 32) continue;

            const Character* glyph = GetGlyph(cell.Codepoint);
            if (!glyph) continue;
            if (glyph->Size.x == 0 || glyph->Size.y == 0) continue;

            float cellX = originX + static_cast<float>(col) * cw;
            float cellY = originY + static_cast<float>(row) * ch;

            float xpos = cellX + static_cast<float>(glyph->Bearing.x);
            float ypos = cellY + (static_cast<float>(_ascender) - static_cast<float>(glyph->Bearing.y));

            float w = static_cast<float>(glyph->Size.x);
            float h = static_cast<float>(glyph->Size.y);

            float r = cell.Fg.r, g = cell.Fg.g, b = cell.Fg.b;

            float vertices[] = {
                xpos,     ypos,     0.0f, 0.0f, r, g, b,
                xpos + w, ypos,     1.0f, 0.0f, r, g, b,
                xpos + w, ypos + h, 1.0f, 1.0f, r, g, b,

                xpos,     ypos,     0.0f, 0.0f, r, g, b,
                xpos + w, ypos + h, 1.0f, 1.0f, r, g, b,
                xpos,     ypos + h, 0.0f, 1.0f, r, g, b,
            };

            glBindTexture(GL_TEXTURE_2D, glyph->TextureID);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float)));
            glEnableVertexAttribArray(2);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void TerminalRenderer::RenderCursor(const TerminalEmulator& emulator, float originX, float originY, const glm::mat4& projection) {
    if ((_frameCount / 30) % 2 != 0) return;

    _bgShader->Use();
    _bgShader->SetMat4(_bgShader->GetLocation("projection"), projection);

    float cw = static_cast<float>(_cellWidth);
    float ch = static_cast<float>(_cellHeight);

    float x = originX + static_cast<float>(emulator.GetCursorX()) * cw;
    float y = originY + static_cast<float>(emulator.GetCursorY()) * ch;

    float r = 0.8f, g = 0.8f, b = 0.8f;

    float vertices[] = {
        x,      y,      r, g, b,
        x + cw, y,      r, g, b,
        x + cw, y + ch, r, g, b,

        x,      y,      r, g, b,
        x + cw, y + ch, r, g, b,
        x,      y + ch, r, g, b,
    };

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

#endif
