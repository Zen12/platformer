#ifndef __EMSCRIPTEN__

#include "terminal_emulator.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstring>

const TerminalCell TerminalEmulator::_emptyCell = {};

static void VtermOutputCallback(const char* s, size_t len, void* user) {
    TerminalEmulator::OutputCallback(s, len, user);
}

void TerminalEmulator::OutputCallback(const char* s, size_t len, void* user) {
    auto* self = static_cast<TerminalEmulator*>(user);
    self->_pty.Write(s, static_cast<int>(len));
}

static int ScrollbackPushLine(int cols, const VTermScreenCell* cells, void* user) {
    auto* self = static_cast<TerminalEmulator*>(user);
    self->PushScrollbackLine(cells, cols);
    return 1;
}

static int ScrollbackPopLine([[maybe_unused]] int cols, [[maybe_unused]] VTermScreenCell* cells, [[maybe_unused]] void* user) {
    return 0;
}

TerminalEmulator::TerminalEmulator(int cols, int rows, const std::string& workingDir)
    : _cols(cols), _rows(rows) {
    _cells.resize(static_cast<size_t>(_cols * _rows));

    _vt = vterm_new(_rows, _cols);
    vterm_set_utf8(_vt, 1);

    vterm_output_set_callback(_vt, VtermOutputCallback, this);

    _vtScreen = vterm_obtain_screen(_vt);
    vterm_screen_enable_altscreen(_vtScreen, 1);
    vterm_screen_set_damage_merge(_vtScreen, VTERM_DAMAGE_SCROLL);

    VTermScreenCallbacks* cbs = new VTermScreenCallbacks{};
    cbs->sb_pushline = ScrollbackPushLine;
    cbs->sb_popline = ScrollbackPopLine;
    vterm_screen_set_callbacks(_vtScreen, cbs, this);

    vterm_screen_reset(_vtScreen, 1);

    _pty.Spawn("", workingDir);
    _pty.Resize(_cols, _rows);
}

TerminalEmulator::~TerminalEmulator() {
    if (_vt) {
        vterm_free(_vt);
        _vt = nullptr;
    }
}

void TerminalEmulator::PushScrollbackLine(const VTermScreenCell* cells, int cols) {
    std::vector<TerminalCell> line(static_cast<size_t>(cols));
    for (int i = 0; i < cols; ++i) {
        line[static_cast<size_t>(i)] = ConvertVtermCell(cells[i]);
    }
    _scrollback.push_back(std::move(line));
    if (static_cast<int>(_scrollback.size()) > MAX_SCROLLBACK) {
        _scrollback.pop_front();
    }
}

TerminalCell TerminalEmulator::ConvertVtermCell(const VTermScreenCell& vtCell) const {
    TerminalCell cell;
    cell.Codepoint = (vtCell.chars[0] != 0) ? vtCell.chars[0] : ' ';
    cell.Bold = vtCell.attrs.bold != 0;

    VTermColor fg = vtCell.fg;
    VTermColor bg = vtCell.bg;
    vterm_screen_convert_color_to_rgb(_vtScreen, &fg);
    vterm_screen_convert_color_to_rgb(_vtScreen, &bg);

    cell.Fg = {
        static_cast<float>(fg.rgb.red) / 255.0f,
        static_cast<float>(fg.rgb.green) / 255.0f,
        static_cast<float>(fg.rgb.blue) / 255.0f
    };
    cell.Bg = {
        static_cast<float>(bg.rgb.red) / 255.0f,
        static_cast<float>(bg.rgb.green) / 255.0f,
        static_cast<float>(bg.rgb.blue) / 255.0f
    };
    return cell;
}

void TerminalEmulator::ProcessOutput() {
    char buf[4096];
    int n = _pty.Read(buf, sizeof(buf));
    bool hadOutput = false;
    while (n > 0) {
        vterm_input_write(_vt, buf, static_cast<size_t>(n));
        vterm_screen_flush_damage(_vtScreen);
        hadOutput = true;
        n = _pty.Read(buf, sizeof(buf));
    }
    if (hadOutput) {
        _scrollOffset = 0;
    }
    SyncFromVterm();
}

void TerminalEmulator::SendChar(uint32_t c) {
    _scrollOffset = 0;
    vterm_keyboard_unichar(_vt, c, VTERM_MOD_NONE);
}

void TerminalEmulator::SendKey(int glfwKey, int mods) {
    _scrollOffset = 0;

    VTermModifier vtMod = VTERM_MOD_NONE;
    if (mods & GLFW_MOD_CONTROL) vtMod = static_cast<VTermModifier>(vtMod | VTERM_MOD_CTRL);
    if (mods & GLFW_MOD_SHIFT)   vtMod = static_cast<VTermModifier>(vtMod | VTERM_MOD_SHIFT);
    if (mods & GLFW_MOD_ALT)     vtMod = static_cast<VTermModifier>(vtMod | VTERM_MOD_ALT);

    VTermKey vtKey = VTERM_KEY_NONE;

    switch (glfwKey) {
        case GLFW_KEY_ENTER:     vtKey = VTERM_KEY_ENTER; break;
        case GLFW_KEY_BACKSPACE: vtKey = VTERM_KEY_BACKSPACE; break;
        case GLFW_KEY_TAB:       vtKey = VTERM_KEY_TAB; break;
        case GLFW_KEY_ESCAPE:    vtKey = VTERM_KEY_ESCAPE; break;
        case GLFW_KEY_UP:        vtKey = VTERM_KEY_UP; break;
        case GLFW_KEY_DOWN:      vtKey = VTERM_KEY_DOWN; break;
        case GLFW_KEY_RIGHT:     vtKey = VTERM_KEY_RIGHT; break;
        case GLFW_KEY_LEFT:      vtKey = VTERM_KEY_LEFT; break;
        case GLFW_KEY_INSERT:    vtKey = VTERM_KEY_INS; break;
        case GLFW_KEY_DELETE:    vtKey = VTERM_KEY_DEL; break;
        case GLFW_KEY_HOME:      vtKey = VTERM_KEY_HOME; break;
        case GLFW_KEY_END:       vtKey = VTERM_KEY_END; break;
        case GLFW_KEY_PAGE_UP:   vtKey = VTERM_KEY_PAGEUP; break;
        case GLFW_KEY_PAGE_DOWN: vtKey = VTERM_KEY_PAGEDOWN; break;
        default: break;
    }

    if (vtKey != VTERM_KEY_NONE) {
        vterm_keyboard_key(_vt, vtKey, vtMod);
        return;
    }

    if ((mods & GLFW_MOD_CONTROL) && glfwKey >= GLFW_KEY_A && glfwKey <= GLFW_KEY_Z) {
        vterm_keyboard_unichar(_vt, static_cast<uint32_t>(glfwKey - GLFW_KEY_A + 'a'), static_cast<VTermModifier>(VTERM_MOD_CTRL));
    }
}

void TerminalEmulator::Scroll(int delta) {
    _scrollOffset += delta;
    int maxScroll = static_cast<int>(_scrollback.size());
    _scrollOffset = std::clamp(_scrollOffset, 0, maxScroll);
}

const TerminalCell& TerminalEmulator::GetCell(int row, int col) const {
    if (col < 0 || col >= _cols || row < 0 || row >= _rows) {
        return _emptyCell;
    }

    if (_scrollOffset == 0) {
        return _cells[static_cast<size_t>(row * _cols + col)];
    }

    int sbSize = static_cast<int>(_scrollback.size());
    int sbRow = sbSize - _scrollOffset + row;

    if (sbRow < 0 || sbRow >= sbSize) {
        if (sbRow >= sbSize) {
            int screenRow = sbRow - sbSize;
            if (screenRow >= 0 && screenRow < _rows) {
                return _cells[static_cast<size_t>(screenRow * _cols + col)];
            }
        }
        return _emptyCell;
    }

    const auto& line = _scrollback[static_cast<size_t>(sbRow)];
    if (col < static_cast<int>(line.size())) {
        return line[static_cast<size_t>(col)];
    }
    return _emptyCell;
}

bool TerminalEmulator::IsAlive() const {
    return _pty.IsAlive();
}

void TerminalEmulator::SyncFromVterm() {
    VTermPos cursorPos;
    vterm_state_get_cursorpos(vterm_obtain_state(_vt), &cursorPos);
    _cursorY = cursorPos.row;
    _cursorX = cursorPos.col;

    VTermScreenCell vtCell;
    for (int row = 0; row < _rows; ++row) {
        for (int col = 0; col < _cols; ++col) {
            VTermPos pos = {row, col};
            vterm_screen_get_cell(_vtScreen, pos, &vtCell);
            _cells[static_cast<size_t>(row * _cols + col)] = ConvertVtermCell(vtCell);
        }
    }
}

#endif
