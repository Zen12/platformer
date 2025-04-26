#pragma once 

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>
#include <iostream>

class Mesh
{
    public:

    private:
        uint32_t _vbo;
        uint32_t _vao;
        uint32_t _ebo;
        std::vector<float> _vertices;
        std::vector<uint32_t> _indices;

    public:
        Mesh() = default;
        Mesh(
            const std::vector<float>& vertices, 
            const std::vector<uint32_t>& indices);

        ~Mesh();

        void Bind() const;
        uint16_t GetVertexCount() const;
        uint16_t GetIndicesCount() const;

        static Mesh GenerateSprite();
};