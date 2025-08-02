#pragma once

#include "spine/Atlas.h"
#include "spine/Skeleton.h"
#include <spine/spine.h>

#include "stb_image.h"
#include "../mesh.hpp"
#include "../shader.hpp"
#include "GL/glew.h"


/// A vertex of a mesh generated from a Spine skeleton
struct vertex_t {
    float x, y;
    uint32_t color;
    float u, v;
    uint32_t darkColor;
};


class GlTextureLoader : public spine::TextureLoader {
public:
    void load(spine::AtlasPage &page, const spine::String &path);
    void unload(void *texture);
};


class SpineData
{
private:
    std::shared_ptr<Shader> shader;
    std::vector<float> vertex_buffer;
    std::vector<uint32_t> index_buffer;
    spine::SkeletonRenderer *renderer;

    spine::Skeleton* skeleton;

public:
    SpineData() {
        // We use a y-down coordinate system, see renderer_set_viewport_size()
        spine::Bone::setYDown(true);

        int width = 800, height = 600;

        // Load the atlas and the skeleton data
        GlTextureLoader textureLoader;
        const auto path = ASSETS_PATH"resources/spines/spineboy-pro.atlas-spine";
        spine::Atlas *atlas = new spine::Atlas(path, &textureLoader);
        spine::SkeletonBinary binary(atlas);
        spine::SkeletonData *skeletonData = binary.readSkeletonDataFile(ASSETS_PATH"resources/spines/spineboy-pro.skel-spine");

        // Create a skeleton from the data, set the skeleton's position to the bottom center of
        // the screen and scale it to make it smaller.
        skeleton = new spine::Skeleton(skeletonData);
        skeleton->setPosition(width / 2, height - 100);
        skeleton->setScaleX(0.3);
        skeleton->setScaleY(0.3);

        // Create an AnimationState to drive animations on the skeleton. Set the "portal" animation
        // on track with index 0.
        spine::AnimationStateData animationStateData(skeletonData);
        animationStateData.setDefaultMix(0.2f);
        spine::AnimationState animationState(&animationStateData);
        animationState.setAnimation(0, "portal", true);
        animationState.addAnimation(0, "run", true, 0);


        const auto vert = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec4 aLightColor;
        layout (location = 2) in vec2 aTexCoord;
        layout (location = 3) in vec4 aDarkColor;

        uniform mat4 uMatrix;

        out vec4 lightColor;
        out vec4 darkColor;
        out vec2 texCoord;

        void main() {
            lightColor = aLightColor;
            darkColor = aDarkColor;
            texCoord = aTexCoord;
            gl_Position = uMatrix * vec4(aPos, 0.0, 1.0);
        }
        )";
        const auto frag =
                                R"(
        #version 330 core
        in vec4 lightColor;
        in vec4 darkColor;
        in vec2 texCoord;
        out vec4 fragColor;

        uniform sampler2D uTexture;
        void main() {
            vec4 texColor = texture(uTexture, texCoord);
            float alpha = texColor.a * lightColor.a;
            fragColor.a = alpha;
            fragColor.rgb = ((texColor.a - 1.0) * darkColor.a + 1.0 - texColor.rgb) * darkColor.rgb + texColor.rgb * lightColor.rgb;
        }
        )";

        renderer = new spine::SkeletonRenderer();
        shader = std::make_shared<Shader>(vert, frag);
        vertex_buffer = std::vector<float>();
    }

    void Render() {

        shader->Use();
        const auto location = shader->GetLocation("uTexture");
        shader->SetInt(location, 0);
        glEnable(GL_BLEND);

        spine::RenderCommand *command = renderer->render(*skeleton);
        while (command) {
            int num_command_vertices = command->numVertices;
            vertex_buffer.clear();
            index_buffer.clear();

            float *positions = command->positions;
            float *uvs = command->uvs;
            uint32_t *colors = command->colors;
            uint32_t *darkColors = command->darkColors;
            for (int i = 0, j = 0; i < num_command_vertices; i++, j += 2) {
                vertex_buffer.push_back(positions[j]);
                vertex_buffer.push_back(positions[j + 1]);
                vertex_buffer.push_back(uvs[j]);
                vertex_buffer.push_back(uvs[j + 1]);
                uint32_t color = colors[i];
                //vertex->color = (color & 0xFF00FF00) | ((color & 0x00FF0000) >> 16) | ((color & 0x000000FF) << 16);
                //uint32_t darkColor = darkColors[i];
                //vertex->darkColor = (darkColor & 0xFF00FF00) | ((darkColor & 0x00FF0000) >> 16) | ((darkColor & 0x000000FF) << 16);
            }

            for (int i = 0; i < command->numIndices; i++) {
                index_buffer.push_back(command->indices[i]);
            }

            //mesh.UpdateData(vertex_buffer, index_buffer);

            GLuint texture = (unsigned int) (uintptr_t)  command->texture;
            glActiveTexture(GL_TEXTURE0);// Set active texture unit to 0
            glBindTexture(GL_TEXTURE_2D, texture);

            //mesh.Bind();
            //glDrawElements(GL_TRIANGLES, mesh.GetIndicesCount(), GL_UNSIGNED_SHORT, nullptr);
            glDrawElements(GL_TRIANGLES, 4, GL_UNSIGNED_SHORT, nullptr);
            glBindVertexArray(0);

            command = command->next;
        }
    }



    void Bind() {
        //_mesh->Bind();
    }

    [[nodiscard]] size_t GetIndicesCount() const {
        return 0;
        //return _mesh->GetIndicesCount();
    }
};
