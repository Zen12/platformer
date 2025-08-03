#include "spine_renderer.hpp"


void SpineRenderer::SetMaterial(const std::weak_ptr<Material> &material) noexcept {
    _material = material;
}

void SpineRenderer::Update() const {
    /*
    if (const auto material = _material.lock()) {
        material->Bind();
        if (const auto spine = _spine.lock()) {
        }
    }
    */
}

void SpineRenderer::Render() noexcept {
    std::cout << "SpineRenderer::Render() \n";
    if (const auto material = _material.lock()) {
        if (const auto spine = _spine.lock()) {
            if (const auto skeleton = spine->GetSkeleton().lock()) {

                std::cout << "Skeleton::Render() \n";

                material->Bind();
                material->SetInt("uTexture", 0);
                glEnable(GL_BLEND);

                spine::RenderCommand *command = _skeletonRenderer->render(*skeleton);
                while (command) {
                    const int num_command_vertices = command->numVertices;

                    std::vector<float> vertices(num_command_vertices * 3);
                    std::vector<uint32_t> index(command->numIndices);

                    float *positions = command->positions;
                    float *uvs = command->uvs;
                    for (int i = 0, j = 0; i < num_command_vertices; i++, j += 2) {
                        vertices.push_back(positions[j]);
                        vertices.push_back(positions[j + 1]);
                        vertices.push_back(uvs[j]);
                        vertices.push_back(uvs[j + 1]);
                    }

                    for (int i = 0; i < command->numIndices; i++) {
                        index.push_back(command->indices[i]);
                    }

                    _mesh.UpdateData(vertices, index);

                    GLuint texture = (unsigned int) (uintptr_t)  command->texture;
                    glActiveTexture(GL_TEXTURE0);// Set active texture unit to 0
                    glBindTexture(GL_TEXTURE_2D, texture);

                    _mesh.Bind();
                    glDrawElements(GL_TRIANGLES, command->numIndices, GL_UNSIGNED_SHORT, nullptr);
                    glBindVertexArray(0);

                    command = command->next;
                }
            }
        }
    }
}
