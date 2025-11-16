#pragma once

// Simple OpenGL render interface for RmlUi
class OpenGLRenderInterface : public Rml::RenderInterface {
private:
    struct GeometryData {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ibo = 0;
        int num_indices = 0;
    };
    std::unordered_map<Rml::CompiledGeometryHandle, std::unique_ptr<Mesh>> geometries;
    Rml::CompiledGeometryHandle next_handle = 1;
    std::vector<GLuint> textures;

    std::weak_ptr<Window> _window;
    std::weak_ptr<Material> _material;


public:
    void Initialize(const std::weak_ptr<Window> window, const std::weak_ptr<Material> material ) {
        _window = window;
        _material = material;
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

        _material.lock()->UseShader();

        // Set projection matrix
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(_window.lock()->GetWidth()),
                                           static_cast<float>(_window.lock()->GetHeight()), 0.0f, -1.0f, 1.0f);

        // Apply translation
        projection = glm::translate(projection, glm::vec3(translation.x, translation.y, 0.0f));

        const auto projectionLoc = _material.lock()->GetLocation("projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        const auto useTextureLoc = _material.lock()->GetLocation("useTexture");

        // Bind texture if present
        if (texture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(texture));
            glUniform1i(useTextureLoc, 1);
        } else {
            glUniform1i(useTextureLoc, 0);
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
        texture_dimensions = {0, 0};
        return 0;
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

        textures.push_back(texture_id);
        return static_cast<Rml::TextureHandle>(texture_id);
    }

    void ReleaseTexture(Rml::TextureHandle texture) override {
        if (texture) {
            GLuint texture_id = static_cast<GLuint>(texture);
            glDeleteTextures(1, &texture_id);
            auto it = std::find(textures.begin(), textures.end(), texture_id);
            if (it != textures.end()) {
                textures.erase(it);
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