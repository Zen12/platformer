#pragma once

#include "../render/font.h"
#include "../render/mesh.h"
#include "../render/material.h"
#include "transforms/transform.h"
#include "transforms/rect_transform.h"
#include "entity.h"

class UiImageRenderer : public Component
{
    private:
        Mesh _mesh;
        std::weak_ptr<Material> _material;

    public:

        UiImageRenderer() = delete;

        UiImageRenderer(const std::weak_ptr<Entity>& entity)
        : Component(entity),
            _mesh(Mesh::GenerateUI())
        {
        }

        void Update() const override;

        void SetMaterial(const std::weak_ptr<Material>& material)
        {
            _material = material;
        }

        void SetSprite(const std::weak_ptr<Sprite>& sprite)
        {
            if (auto material = _material.lock())
            {
                material->AddSprite(sprite);
            }
        }
        void Render(const glm::mat4& projection) const;

    private:
        void Bind();
};