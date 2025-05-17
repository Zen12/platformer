#include "ui_image_renderer.h"

uint32_t VAO, VBO;


void UiImageRenderer::Render(const glm::mat4& projection) const
{
    const float width = 2.0f / projection[0][0];
    const float height = 2.0f / projection[1][1];

    if (auto material = _material.lock())
    {
        material->SetMat4("projection", projection);
    }
    
    const auto position = GetEntity().lock()->GetComponent<RectTransform>().lock()->GetAnchoredPosion();

    const float xpos = position.x * width;
    const float ypos = position.y * height;
    const float w = position.z * width;
    const float h = position.w * height;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 0.0f },            
        { xpos,     ypos,       0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 1.0f },

        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos + w, ypos,       1.0f, 1.0f },
        { xpos + w, ypos + h,   1.0f, 0.0f }           
    };

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}


