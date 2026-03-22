#pragma once // Prevents the compiler from including this file twice

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

// A strict, lightweight struct perfectly mapped to our shader inputs
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Color;
};

class Mesh
{
public:
    void updateGPUData();

    std::vector<Vertex> vertices;
    unsigned int VAO, VBO;
    GLenum drawMode; // Added this

    // Update constructor signature
    Mesh(std::vector<Vertex> vertices, GLenum mode = GL_TRIANGLES);
    void Draw();

private:
    void setupMesh();
};