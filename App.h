#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <vector>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Mesh.h"
#include "Point.h"
#include "Line.h"
#include "SplitTool.h"
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
    std::vector<Line> modelLines;
    
    // UI/Interaction State
    bool isRunning;
    bool leftDown, midDown, ctrlHeld, isAnimatingCamera, draggingPoints;
    bool showGrid, pointMode;
    glm::vec2 selStart, selEnd;
    float targetYaw, targetPitch;

    // Undo/Redo State
    struct EditState {
        std::vector<Point> points;
        std::vector<unsigned int> indices;
        std::vector<Line> lines; // Line selections are now snapshot safe!
    };
    std::vector<EditState> history;
    int historyIndex;
    bool hasUnsavedChanges;
    std::vector<int> selectedIndices; // Tracks the exact order points are selected

    // Helper Methods
    void saveState();
    void undo();
    void redo();

    void processEvents();
    void update();
    void buildUI();
    void render();
    void cleanup();
    
    void extractLinesFromMesh();
    void initShaders();
    void initGeometry();
};