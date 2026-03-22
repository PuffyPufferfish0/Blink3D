#pragma once // Prevents the compiler from including this file twice

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

// A strict, lightweight struct perfectly mapped to our shader inputs
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Color;
};

class Mesh {
public:
    // We use std::vector so the memory is contiguous, just like a raw array
    std::vector<Vertex> vertices;
    
    // OpenGL buffer IDs
    unsigned int VAO, VBO;

    // Constructor
    Mesh(std::vector<Vertex> vertices);

    // Render the mesh
    void Draw();

private:
    // Internal function to bind the data to the GPU
    void setupMesh();
};