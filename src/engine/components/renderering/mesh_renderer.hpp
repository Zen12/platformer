#pragma once
#include "../entity.hpp"
#include "../../debug/debug.hpp"
#include "../../render/material.hpp"
#include "../../render/mesh.hpp"
#include "../../render/line.hpp"
#include "../../renderer/transform/transform_component.hpp"

#define DEBUG_ENGINE_MESH_RENDERER 0


class MeshRenderer final : public Component {

private:
    Mesh _mesh;
    std::weak_ptr<Material> _material{};
    std::weak_ptr<Sprite> _sprite{};
    std::weak_ptr<Transform> _transform{};
    std::unordered_map<std::string, glm::vec3> _vec3Cache;
    std::unordered_map<std::string, float> _floatCache;

public:
    explicit MeshRenderer(const std::weak_ptr<Entity> &entity)
        : Component(entity), _mesh(Mesh::GenerateSprite())
    {
        _transform = _entity.lock()->GetComponent<Transform>();
    }

    void SetSprite(const std::weak_ptr<Sprite> &sprite) {
        _sprite = sprite;
    }

    void UpdateMesh(const std::vector<float> &vertices, const std::vector<unsigned int> &indices) {
        _mesh.UpdateData(vertices, indices);
    }

    void SetUniformMat4(const std::string& name, const glm::mat4 &m) const noexcept {
        if (const auto material = _material.lock()) {
            material->SetMat4(name, m);
        }
    }

    void SetUniformVec3(const std::string& name, const glm::vec3 &value) noexcept {
        _vec3Cache [name] = value;
    }

    void SetUniformFloat(const std::string& name, const float &value) noexcept {
        _floatCache [name] = value;
    }


    void SetMaterial(std::weak_ptr<Material> material) {
        _material = std::move( material);
    }

    void Update([[maybe_unused]] const float& deltaTime) override {

    }

    void Render() const noexcept {

        if (const auto material = _material.lock()) {
            if (const auto sprite = _sprite.lock()) {
                sprite->Bind();
            }
            material->Bind();
            _mesh.Bind();

            for (const auto &[name, value] : _vec3Cache) {
                material->SetVec3(name, value.x, value.y, value.z);
            }
            for (const auto &[name, value] : _floatCache) {
                material->SetFloat(name, value);
            }

            glDrawElements(GL_TRIANGLES, static_cast<int32_t>(_mesh.GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
        }


    }

    [[nodiscard]] float GetZOrder() const noexcept {
        if (const auto transform = _transform.lock()) {
            return transform->GetPosition().z;
        }

        return 0;
    }

#ifndef NDEBUG
    void DrawDebug() {
        const auto verts = _mesh.GetVertices();
        for (int i=0; i< verts.size(); i+=5) {
            const glm::vec3 target  = glm::vec3(verts[i], verts[i+1], verts[i+2]);
            DebugLines::AddLine(_transform.lock()->GetPosition(), target);
        }
    }

#endif

    [[nodiscard]] glm::mat4 GetModel() const noexcept {
        if (const auto transform = _transform.lock()) {
            return transform->GetModel();
        }
        return {};
    }
};
