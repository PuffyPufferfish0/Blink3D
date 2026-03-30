#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <vector>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Mesh.h"
#include "Point.h"
#include "ViewCube.h"
#include "Toolbar.h"
#include "TransformGizmo.h"

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
    ViewCube* viewCube;
    Toolbar* toolbar;
    TransformGizmo* gizmo;
    Mesh* myMesh;
    Mesh* grid;
    std::vector<Point> modelPoints;
    
    // UI/Interaction State
    bool isRunning;
    bool leftDown, midDown, ctrlHeld, isAnimatingCamera, draggingPoints;
    bool showGrid, pointMode;
    glm::vec2 selStart, selEnd;
    float targetYaw, targetPitch;

    // Helper Methods
    void processEvents();
    void update();
    void buildUI();
    void render();
    void cleanup();
    
    void initShaders();
    void initGeometry();
};