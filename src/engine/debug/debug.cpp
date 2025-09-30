#include "debug.hpp"

#include "../renderer/material/material.hpp"


static std::vector<std::shared_ptr<Line>> g_lines{};
static std::shared_ptr<Shader> g_shader;
static std::shared_ptr<Material> g_material;
static glm::mat4 g_projection;
static glm::mat4 g_view;

const std::string g_vertexSource_opengl = R"(#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 projection;
uniform mat4 view;

out vec3 color;

void main() {
    color = aColor;
    gl_Position = projection * view * vec4(aPos, 1.0);
}


)";

const std::string g_fragmentSource_opengl = R"(#version 330 core

out vec4 FragColor;

in vec3 color;

void main() {
    FragColor = vec4(color.x, color.y, color.z, 1.0);
}

)";

const std::string g_vertexSource_gles = R"(#version 300 es

precision mediump float;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 projection;
uniform mat4 view;

out vec3 color;

void main() {
    color = aColor;
    gl_Position = projection * view * vec4(aPos, 1.0);
}

)";

const std::string g_fragmentSource_gles = R"(#version 300 es

precision mediump float;

out vec4 FragColor;

in vec3 color;

void main() {
    FragColor = vec4(color.x, color.y, color.z, 1.0);
}

)";

void DebugLines::Init() {
    Clear();
    g_lines.reserve(1024);
#ifdef __EMSCRIPTEN__
    g_shader = std::make_shared<Shader>(g_vertexSource_gles, g_fragmentSource_gles);
#else
    g_shader = std::make_shared<Shader>(g_vertexSource_opengl, g_fragmentSource_opengl);
#endif
    g_material = std::make_shared<Material>(g_shader);
}

void DebugLines::UpdateViewProjection(const glm::mat4 &view, const glm::mat4 &projection) {
    g_projection = projection;
    g_view = view;
}

void DebugLines::AddLine(const glm::vec3 &start, const glm::vec3 &end) {
    AddLine(start, end, glm::vec3(1.0, 0.0, 0.0)); // red color
}

void DebugLines::AddLine(const glm::vec2 &start, const glm::vec2 &end) {
    AddLine(glm::vec3(start.x, start.y, 0),  glm::vec3(end.x, end.y, 0), glm::vec3(1.0, 0.0, 0.0)); // red color
}

void DebugLines::AddLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color) {
    const auto lines = Line::GenerateLine(start, end, color);
    g_lines.push_back(lines);
}

void DebugLines::DrawLines() {

    for (const auto &line: g_lines) {
        g_material->Bind();
        line->Bind();

        g_material->SetMat4("view", g_view);
        g_material->SetMat4("projection", g_projection);
        glDrawElements(GL_TRIANGLES, static_cast<int32_t>(line->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
    }
}

void DebugLines::Clear() {
    g_lines.clear();
}

