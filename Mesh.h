#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Color;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    GLenum drawMode;

    Mesh(std::vector<Vertex> vertices, GLenum mode = GL_TRIANGLES, std::vector<unsigned int> indices = {});
    void Draw();
    void updateGPUData();

private:
    void setupMesh();
};