#pragma once

#include "RmlUi/Core/RenderInterface.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../mesh/mesh.hpp"
#include "../material/material.hpp"
#include "../texture/texture.hpp"
#include "window.hpp"
#include "loaders/asset_texture_loader.h"
#include "asset_manager.hpp"
#include "guid.hpp"

class UiOpenGLRenderController : public Rml::RenderInterface {
private:
    std::unordered_map<Rml::CompiledGeometryHandle, std::unique_ptr<Mesh>> geometries;
    Rml::CompiledGeometryHandle next_handle = 1;

    std::weak_ptr<Window> _window;
    std::weak_ptr<Material> _material;
    std::weak_ptr<AssetManager> _assetManager;

    std::vector<std::shared_ptr<Texture>> _textures{};
    AssetTextureLoader _texture_loader;

    std::string ResolveGuidToPath(const std::string& path) const {
        constexpr const char* guidPrefix = "guid://";
        constexpr size_t guidPrefixLen = 7;

        if (path.rfind(guidPrefix, 0) == 0) {
            const std::string guidStr = path.substr(guidPrefixLen);
            const Guid guid = Guid::FromString(guidStr);

            if (const auto assetManager = _assetManager.lock()) {
                return assetManager->GetPathFromGuid(guid);
            }

            std::cerr << "AssetManager not available for GUID resolution: " << guidStr << std::endl;
            return "";
        }

        return path;
    }


public:
    void Initialize(const std::weak_ptr<Window> &window, const std::weak_ptr<Material> &material,
                    const std::weak_ptr<AssetManager> &assetManager) {
        _window = window;
        _material = material;
        _assetManager = assetManager;
    }

    Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
                                                Rml::Span<const int> indices) override {
        const auto handle = next_handle++;
        geometries[handle] = std::make_unique<Mesh>(vertices, indices);
        return handle;
    }

    void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation,
                      Rml::TextureHandle texture) override {
        auto it = geometries.find(geometry);
        if (it == geometries.end()) return;

        const auto& data = it->second;

        const auto material = _material.lock();
        if (!material) return;

        material->Bind();

        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(_window.lock()->GetWidth()),
                                           static_cast<float>(_window.lock()->GetHeight()), 0.0f, -1.0f, 1.0f);

        projection = glm::translate(projection, glm::vec3(translation.x, translation.y, 0.0f));
        material->SetMat4("projection",projection);

        if (texture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(texture));
            material->SetInt("useTexture", 1);
        } else {
            material->SetInt("useTexture", 0);
        }

        glBindVertexArray(data->VAO());
        glDrawElements(GL_TRIANGLES, data->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        if (texture) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glUseProgram(0);
    }
    void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override {
        geometries[geometry] = nullptr;
    }

    Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions,
                                  const Rml::String& source) override {
        const std::string resolvedPath = ResolveGuidToPath(source);

        if (resolvedPath.empty()) {
            texture_dimensions = {0, 0};
            return 0;
        }

        int width, height;
        GLuint texture_id = _texture_loader.texture_load(resolvedPath.c_str(), false, &width, &height);

        if (texture_id == 0) {
            texture_dimensions = {0, 0};
            return 0;
        }

        texture_dimensions = {width, height};

        const auto texture = std::make_shared<Texture>(texture_id);
        _textures.push_back(texture);
        if (const auto material = _material.lock()) {
            material->AddSprite(texture);
        }

        return static_cast<Rml::TextureHandle>(texture_id);
    }

    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source,
                                      Rml::Vector2i source_dimensions) override {
        GLuint texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, source_dimensions.x, source_dimensions.y,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, source.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);

        const auto texture = std::make_shared<Texture>(texture_id);
        _textures.push_back(texture);
        _material.lock()->AddSprite(texture);
        return static_cast<Rml::TextureHandle>(texture_id);
    }

    void ReleaseTexture(Rml::TextureHandle texture) override {
        if (texture) {
            GLuint texture_id = static_cast<GLuint>(texture);
            glDeleteTextures(1, &texture_id);
            const auto it = std::find_if(_textures.begin(), _textures.end(),
                                   [texture_id](const std::shared_ptr<Texture>& t) {
                                       return t->GetTextureId() == texture_id;
                                   });

            if (it != _textures.end()) {
                _textures.erase(it);
            }
        }
    }

    void EnableScissorRegion(bool enable) override {
        if (enable) glEnable(GL_SCISSOR_TEST);
        else glDisable(GL_SCISSOR_TEST);
    }

    void SetScissorRegion(Rml::Rectanglei region) override {
        glScissor(region.Left(), region.Top(), region.Width(), region.Height());
    }
};
