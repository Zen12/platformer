#pragma once
#include "entity.hpp"
#include "../render/material.hpp"
#include "../render/mesh.hpp"


class MeshRenderer final : public Component {

private:
    Mesh _mesh;
    std::weak_ptr<Material> _material{};

public:
    explicit MeshRenderer(const std::weak_ptr<Entity> &entity)
        : Component(entity), _mesh(Mesh::GenerateSquare())
    {
    }


    void UpdateMesh(const std::vector<float> &vertices, const std::vector<unsigned int> &indices) {
        _mesh.UpdateData(vertices, indices);
    }


    void SetMaterial(std::weak_ptr<Material> material) {
        _material = std::move( material);
    }

    void Update() const override {
        if (const auto material = _material.lock()) {
            material->Bind();
            _mesh.Bind();
        }
    }

    void Render() const noexcept {
        if (const auto material = _material.lock()) {
            glDrawElements(GL_TRIANGLES, static_cast<int32_t>(_mesh.GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
        }
    }

    [[nodiscard]] int32_t GetShaderId() const noexcept {
        if (const auto material = _material.lock()) {
            return material->GetShaderId();
        }

        return -1;
    }
};
