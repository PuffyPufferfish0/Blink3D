#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <vector>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Mesh.h"
#include "Point.h"
#include "Line.h"
#include "Face.h"
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
    SDL_Window* window;
    SDL_GLContext glContext;
    GLuint shaderProg;
    
    Camera* camera;
    ViewCube* viewCube;
    Toolbar* toolbar;
    TransformGizmo* gizmo;
    Mesh* myMesh;
    Mesh* grid;
    std::vector<Point> modelPoints;
    std::vector<Line> modelLines;
    std::vector<Face> modelFaces; // Added Faces
    
    bool isRunning;
    bool leftDown, midDown, ctrlHeld, isAnimatingCamera, draggingPoints;
    bool showGrid, pointMode, showPreferencesWindow; // Added showPreferencesWindow
    glm::vec2 selStart, selEnd;
    float targetYaw, targetPitch;

    struct EditState {
        std::vector<Point> points;
        std::vector<unsigned int> indices;
        std::vector<Line> lines;
        std::vector<Face> faces; // Added Faces to undo state
    };
    std::vector<EditState> history;
    int historyIndex;
    bool hasUnsavedChanges;
    std::vector<int> selectedIndices; 

    void saveState();
    void undo();
    void redo();

    void processEvents();
    void update();
    void buildUI();
    void render();
    void cleanup();
    
    void extractTopologyFromMesh();
    void initShaders();
    void initGeometry();
};