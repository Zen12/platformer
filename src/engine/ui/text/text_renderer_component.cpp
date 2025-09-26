#include "text_renderer_component.hpp"


void UiTextRendererComponent::Bind()
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

UiTextRendererComponent::UiTextRendererComponent(const std::weak_ptr<Entity> &entity) : Component(entity)
{
    Bind();
}

void UiTextRendererComponent::Update([[maybe_unused]] const float& deltaTime)
{
}

void UiTextRendererComponent::Render(const glm::mat4 &projection)
{
    if (const auto &material = _material.lock())
    {
        material->Bind();
        material->SetVec3("textColor", _color.x, _color.y, _color.z);

        if (const auto font = material->GetFont().lock())
        {
            const auto model = GetEntity().lock()->GetComponent<RectTransform>().lock()->GetModel();

            material->SetMat4("projection", projection);
            material->SetMat4("model", model);


            const auto modelScaleX = glm::length(glm::vec3(model[0]));
            const auto modelScaleY = glm::length(glm::vec3(model[1]));

            // inject
            float x = 0.0f;
            float y = 0.0f;

            const float scaleX = _fontSize / modelScaleX;
            const float scaleY = _fontSize / modelScaleY;

            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(VAO);
            float totalWidth = 0.0f;
            float totalHeight = 0.0f;

            for (char & c : _text) {
                const Character ch = font->Characters.at(c);
                totalWidth += static_cast<float>(ch.Advance >> 6) * scaleX;
                totalHeight = static_cast<float>(ch.Size.y) * scaleY;
            }

            x = 0.5f - totalWidth / 2;
            y = 0.5f - totalHeight / 2;

            for (const char &c : _text)
            {
                const Character ch = font->Characters.at(c);

                const float xpos = x + static_cast<float>(ch.Bearing.x) * scaleX;
                const float ypos = y - static_cast<float>(ch.Size.y - ch.Bearing.y) * scaleY;

                const float w = static_cast<float>(ch.Size.x) * scaleX;
                const float h = static_cast<float>(ch.Size.y) * scaleY;
                // update VBO for each character
                const float vertices[6][4] = {
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
                x += static_cast<float>(ch.Advance >> 6) * scaleX; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
            }
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}
