#include "debug.hpp"

static std::vector<std::shared_ptr<Line>> _lines;

void DebugLines::AddLine(const glm::vec3 &start, const glm::vec3 &end) {
    const auto lines = Line::GenerateLine(start, end);
    _lines.push_back(lines);
}

void DebugLines::DrawLines() {
    for (const auto &line: _lines) {
        line->Bind();
    }
}

void DebugLines::Clear() {
    _lines.clear();
}
