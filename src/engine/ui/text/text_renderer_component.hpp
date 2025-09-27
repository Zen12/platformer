#pragma once

#include "../../render/material.hpp"
#include "../../asset/loaders/asset_loader.hpp"
#include "../../components/entity.hpp"
#include "../rect_transform/rect_transform_component.hpp"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class UiTextRendererComponent final : public Component
{
private:
    std::string _text{};
    uint32_t VAO{}, VBO{};
    float _fontSize{1};
    std::weak_ptr<Material> _material{};
    glm::vec3 _color{};

public:
    explicit UiTextRendererComponent(const std::weak_ptr<Entity> &entity);

    void SetText(const std::string &text) { _text = text; }
    void SetMaterial(const std::weak_ptr<Material> &material) { _material = material; }
    void SetColor(const glm::vec3 &color) { _color = color; }
    void SetFontSize(const float& fontSize) { _fontSize = fontSize; }

    void Update([[maybe_unused]] const float& deltaTime) override;
    void Render(const glm::mat4 &projection);

    [[nodiscard]] int32_t GetShaderId() const { return _material.lock()->GetShaderId(); }

private:
    void Bind();
};