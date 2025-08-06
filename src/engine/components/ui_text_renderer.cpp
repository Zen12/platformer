#include "ui_text_renderer.hpp"

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

void UiTextRenderer::Bind()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

UiTextRenderer::UiTextRenderer(const std::weak_ptr<Entity> &entity) : Component(entity)
{
    Bind();
}

void UiTextRenderer::Update([[maybe_unused]] const float& deltaTime)
{
    if (auto material = _material.lock())
    {
        constexpr glm::vec3 color = glm::vec3(0.5, 0.8f, 0.2f);

        material->Bind();
        material->SetVec3("textColor", color.x, color.y, color.z);
    }
}

void UiTextRenderer::Render(const glm::mat4 &projection)
{
    if (const auto material = _material.lock())
    {
        if (const auto font = material->GetFont().lock())
        {
            const auto model = GetEntity().lock()->GetComponent<RectTransform>().lock()->GetModel();

            material->SetMat4("projection", projection);
            material->SetMat4("model", model);

            // inject
            float x = 0.0f;
            float y = 0.0f;

            const float scale = _fontSize;

            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(VAO);

            for (char & c : _text)
            {
                const Character ch = font->Characters.at(c);

                const float xpos = x + ch.Bearing.x * scale;
                const float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

                float w = ch.Size.x * scale;
                float h = ch.Size.y * scale;
                // update VBO for each character
                float vertices[6][4] = {
                    {xpos, ypos + h, 0.0f, 0.0f},
                    {xpos, ypos, 0.0f, 1.0f},
                    {xpos + w, ypos, 1.0f, 1.0f},

                    {xpos, ypos + h, 0.0f, 0.0f},
                    {xpos + w, ypos, 1.0f, 1.0f},
                    {xpos + w, ypos + h, 1.0f, 0.0f}};
                // render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                // update content of VBO memory
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                // render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
            }
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}
