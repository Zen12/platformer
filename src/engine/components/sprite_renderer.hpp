#pragma once

#include "entity.hpp"
#include "../asset/loaders/asset_loader.hpp"
#include "../render/mesh.hpp"
#include "../render/material.hpp"
#include "transforms/transform.hpp"

class SpriteRenderer : public Component
{

private:
    std::weak_ptr<Sprite> _sprite;
    Mesh _mesh;
    std::weak_ptr<Material> _material;
    std::weak_ptr<Transform> _transform;

public:
    SpriteRenderer() = delete;
    explicit SpriteRenderer(const std::weak_ptr<Entity> &entity)
    : Component(entity),
      _mesh(Mesh::GenerateSprite()),
        _transform(entity.lock()->GetComponent<Transform>())
    {}

    void Update([[maybe_unused]] const float& deltaTime) override;

    void SetSprite(std::weak_ptr<Sprite> sprite) noexcept;

    void SetMaterial(std::weak_ptr<Material> material) noexcept;

    void SetUniformMat4(const std::string& name, const glm::mat4 &m) const noexcept {
        if (const auto material = _material.lock()) {
            material->SetMat4(name, m);
        }
    }

    [[nodiscard]] glm::mat4 GetModel() const noexcept {
        if (const auto transform = _transform.lock()) {
            return transform->GetModel();
        }
        return {};
    }

    void Render() const noexcept;

    ~SpriteRenderer() override = default;
};
