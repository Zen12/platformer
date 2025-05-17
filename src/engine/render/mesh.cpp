#include "mesh.h"
#include <GL/glew.h>


Mesh::Mesh(
    const std::vector<float>& vertices, 
    const std::vector<uint32_t>& indices)
{
    _vertices = vertices;
    _indices = indices;

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(_vao);
 
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);


    uint32_t stride = 5 * sizeof(float);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0); 
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
 
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0); 
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
}

void Mesh::Bind() const
{
    glBindVertexArray(_vao); 
}

uint16_t Mesh::GetVertexCount() const
{
    return _vertices.size();
}

uint16_t Mesh::GetIndicesCount() const
{
    return _indices.size();
}

Mesh Mesh::GenerateSprite()
{
        // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<float> vertices = {
    // positions                // texture coords
        0.5f,  0.5f, 0.0f,      1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,      1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,      0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,      0.0f, 1.0f    // top left 
    };
   std::vector<uint32_t> indices = { 
       0, 3, 1, 
       1, 3, 2   
   };

    return Mesh(vertices, indices);;
}

Mesh Mesh::GenerateUI()
{
    std::vector<float> vertices = {
    // positions                // texture coords
        1.0f,  1.0f, 0.0f,      1.0f, 1.0f,   // top right
        1.0f,  0.0f, 0.0f,      1.0f, 0.0f,   // bottom right
        0.0f,  0.0f, 0.0f,      0.0f, 0.0f,   // bottom left
        0.0f,  1.0f, 0.0f,      0.0f, 1.0f    // top left 
    };
   std::vector<uint32_t> indices = { 
       0, 3, 1, 
       1, 3, 2   
   };

    return Mesh(vertices, indices);;
}

