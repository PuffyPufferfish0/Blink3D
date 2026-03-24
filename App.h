#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <vector>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Mesh.h"
#include "Point.h"

class App {
public:
    App();
    ~App();

    bool init();
    void run();

private:
    // Core Systems
    SDL_Window* window;
    SDL_GLContext glContext;
    GLuint shaderProg;
    
    // Editor State
    Camera* camera;
    Mesh* myMesh;
    Mesh* grid;
    std::vector<Point> modelPoints;
    
    // UI/Interaction State
    bool isRunning;
    bool leftDown, midDown, ctrlHeld;
    bool showGrid, pointMode;
    glm::vec2 selStart, selEnd;

    // Helper Methods
    void processEvents();
    void update();
    void buildUI(); // Renamed to accurately reflect ImGui workflow
    void render();
    void cleanup();
    
    void initShaders();
    void initGeometry();
};