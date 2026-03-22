#include "Mesh.h"

// Constructor takes the list of vertices and immediately sends them to the GPU
Mesh::Mesh(std::vector<Vertex> vertices) {
    this->vertices = vertices;
    setupMesh();
}

void Mesh::setupMesh() {
    // 1. Generate the buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 2. Bind the VAO
    glBindVertexArray(VAO);

    // 3. Bind and load the VBO with our vector data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Because std::vector holds memory contiguously, we can just pass the pointer to the first element
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // 4. Tell OpenGL how to read the Position data (Location 0)
    // offsetof is a great C++ macro that automatically calculates the byte offset of a struct member
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // 5. Tell OpenGL how to read the Color data (Location 1)
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

    // Unbind the VAO so we don't accidentally modify it later
    glBindVertexArray(0);
}

void Mesh::Draw() {
    // Bind the specific VAO for this mesh and draw its triangles
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
}