#include "debug.hpp"

#include "../render/material.hpp"

static std::vector<std::shared_ptr<Line>> _lines;
static std::shared_ptr<Shader> _shader;
static std::shared_ptr<Material> _material;
static glm::mat4 _projection;
static glm::mat4 _view;

const std::string vertexSource = R"(

#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
}


)";

const std::string fragmentSource = R"(

#version 330 core

out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // red
}

)";

void DebugLines::Init() {
    Clear();
    _lines.reserve(1024);

    _shader = std::make_shared<Shader>(vertexSource, fragmentSource);
    _material = std::make_shared<Material>(_shader);
}

void DebugLines::UpdateViewProjection(const glm::mat4 &view, const glm::mat4 &projection) {
    _projection = projection;
    _view = view;
}

void DebugLines::AddLine(const glm::vec3 &start, const glm::vec3 &end) {
    const auto lines = Line::GenerateLine(start, end);
    _lines.push_back(lines);
}

void DebugLines::DrawLines() {

    for (const auto &line: _lines) {
        _material->Bind();
        line->Bind();

        _material->SetMat4("view", _view);
        _material->SetMat4("projection", _projection);
        glDrawElements(GL_TRIANGLES, static_cast<int32_t>(line->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
    }
}

void DebugLines::Clear() {
    _lines.clear();
}
