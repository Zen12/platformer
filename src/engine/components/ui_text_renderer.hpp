#pragma once

#include "../render/font.hpp"
#include "../render/mesh.hpp"
#include "../render/material.hpp"
#include "../asset/asset_loader.hpp"
#include "transforms/transform.hpp"
#include "transforms/rect_transform.hpp"
#include "entity.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class UiTextRenderer : public Component
{
private:
    std::string _text{};
    uint32_t VAO{}, VBO{};
    float _fontSize = 1;
    std::weak_ptr<Material> _material{};

public:
    explicit UiTextRenderer(const std::weak_ptr<Entity> &entity);

    void SetText(const std::string &text)
    {
        _text = text;
    };
    void SetMaterial(const std::weak_ptr<Material> &material)
    {
        _material = material;
    }

    void Update() const override;
    void Render(const glm::mat4 &projection);

    [[nodiscard]] int32_t GetShaderId() const { return _material.lock()->GetShaderId(); }

private:
    void Bind();
};