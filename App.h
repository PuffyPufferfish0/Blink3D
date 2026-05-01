#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <fstream>
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
#include "AddMenu.h"

struct AppConfig {
    char exportDirectory[256] = "./export";
    glm::vec3 bgColor = glm::vec3(0.12f, 0.12f, 0.15f);
    glm::vec3 unselectedPointColor = glm::vec3(0.0f, 1.0f, 1.0f);
    glm::vec3 selectedPointColor = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 unselectedLineColor = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 selectedLineColor = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 unselectedFaceColor = glm::vec3(0.45f, 0.45f, 0.45f);
    glm::vec3 selectedFaceColor = glm::vec3(0.9f, 0.9f, 0.2f);
    float pointSize = 20.0f;
    float lineThickness = 1.0f;
    int windowResIndex = 1; // Default to 1280x720
};

struct ClipboardData {
    std::vector<glm::vec3> points;
    std::vector<unsigned int> indices;
};

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
    AddMenu* addMenu;
    Mesh* myMesh;
    Mesh* grid;
    std::vector<Point> modelPoints;
    std::vector<Line> modelLines;
    std::vector<Face> modelFaces;
    
    AppConfig config;
    ClipboardData clipboard;
    
    bool isRunning;
    bool leftDown, midDown, ctrlHeld, isAnimatingCamera, draggingPoints;
    bool showGrid, pointMode, showPreferencesWindow;
    glm::vec2 selStart, selEnd;
    float targetYaw, targetPitch;

    struct EditState {
        std::vector<Point> points;
        std::vector<unsigned int> indices;
        std::vector<Line> lines;
        std::vector<Face> faces;
    };
    std::vector<EditState> history;
    int historyIndex;
    bool hasUnsavedChanges;
    std::vector<int> selectedIndices; 

    void loadConfig();
    void saveConfig();
    void applyWindowResolution();

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