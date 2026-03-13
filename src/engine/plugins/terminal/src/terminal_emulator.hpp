#pragma once

#ifndef __EMSCRIPTEN__

#include <deque>
#include <string>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

#include "pty_process.hpp"

extern "C" {
#include <vterm.h>
}

struct TerminalCell {
    uint32_t Codepoint = ' ';
    glm::vec3 Fg = {1.0f, 1.0f, 1.0f};
    glm::vec3 Bg = {0.12f, 0.12f, 0.12f};
    bool Bold = false;
};

class TerminalEmulator {
public:
    TerminalEmulator(int cols, int rows, const std::string& workingDir = "");
    ~TerminalEmulator();

    TerminalEmulator(const TerminalEmulator&) = delete;
    TerminalEmulator& operator=(const TerminalEmulator&) = delete;

    void ProcessOutput();
    void SendChar(uint32_t c);
    void SendKey(int glfwKey, int mods);
    void Scroll(int delta);

    [[nodiscard]] const TerminalCell& GetCell(int row, int col) const;
    [[nodiscard]] int GetCols() const noexcept { return _cols; }
    [[nodiscard]] int GetRows() const noexcept { return _rows; }
    [[nodiscard]] int GetCursorX() const noexcept { return _cursorX; }
    [[nodiscard]] int GetCursorY() const noexcept { return _cursorY; }
    [[nodiscard]] int GetScrollOffset() const noexcept { return _scrollOffset; }
    [[nodiscard]] bool IsAlive() const;

    static void OutputCallback(const char* s, size_t len, void* user);

    void PushScrollbackLine(const VTermScreenCell* cells, int cols);

private:
    void SyncFromVterm();
    TerminalCell ConvertVtermCell(const VTermScreenCell& vtCell) const;

    std::vector<TerminalCell> _cells;
    std::deque<std::vector<TerminalCell>> _scrollback;
    int _scrollOffset = 0;
    int _cols;
    int _rows;
    int _cursorX = 0;
    int _cursorY = 0;

    PtyProcess _pty;

    VTerm* _vt = nullptr;
    VTermScreen* _vtScreen = nullptr;

    static const TerminalCell _emptyCell;
    static constexpr int MAX_SCROLLBACK = 5000;
};

#endif
