#pragma once

#include "../render/font.h"
#include "../render/mesh.h"
#include "../render/material.h"
#include "../components/transform.h"
#include "../components/rect_transform.h"
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
            _mesh(Mesh::GenerateSprite())
        {
        }

        void Update() const override
        {
            if (auto material = _material.lock())
            {
                material->Bind();
            }
        }

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

        const uint32_t GetShaderId() const
        {
            if (auto material = _material.lock())
            {
                return material->GetShaderId();
            }

            return -1;
        }

    private:
        void Bind();
};