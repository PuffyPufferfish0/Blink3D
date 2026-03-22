#include "Camera.h"
#include "Mesh.h"
#include "Point.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <glad/glad.h> 
#include <SDL2/SDL.h>
#include <algorithm>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// --- Shaders ---
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "uniform bool isPoint;\n"
    "uniform bool overrideColor;\n"
    "uniform vec3 colorVec;\n"
    "void main()\n"
    "{\n"
    "   if(isPoint) {\n"
    "       vec2 circCoord = gl_PointCoord - vec2(0.5);\n"
    "       float distSq = dot(circCoord, circCoord);\n"
    "       if(distSq > 0.25) discard;\n"
    "       // Add a thin border to the circles for visibility\n"
    "       if(distSq > 0.20) {\n"
    "           FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "           return;\n"
    "       }\n"
    "   }\n"
    "   if(overrideColor) FragColor = vec4(colorVec, 1.0f);\n"
    "   else FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";

// --- Helpers ---
glm::vec2 ProjectToScreen(glm::vec3 p, glm::mat4 v, glm::mat4 pr, int w, int h) {
    glm::vec4 clip = pr * v * glm::vec4(p, 1.0f);
    if (clip.w <= 0) return glm::vec2(-1);
    glm::vec3 ndc = glm::vec3(clip) / clip.w;
    return glm::vec2((ndc.x + 1.0f) * 0.5f * (float)w, (1.0f - ndc.y) * 0.5f * (float)h);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window = SDL_CreateWindow("Blink3D Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vShader);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fShader);
    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vShader);
    glAttachShader(shaderProg, fShader);
    glLinkProgram(shaderProg);

    // --- Scene Logic ---
    std::vector<Point> modelPoints;
    for(float x=-0.5f; x<=0.5f; x+=1.0f)
        for(float y=-0.5f; y<=0.5f; y+=1.0f)
            for(float z=-0.5f; z<=0.5f; z+=1.0f)
                modelPoints.push_back(Point(glm::vec3(x,y,z)));

    std::vector<Vertex> meshVerts;
    for(auto& p : modelPoints) meshVerts.push_back({p.position, p.color});
    Mesh myMesh(meshVerts);

    // Grid Generation
    std::vector<Vertex> gridVertices;
    float gridSize = 10.0f;
    int divisions = 20;
    glm::vec3 gridColor = glm::vec3(0.3f, 0.3f, 0.3f);
    for (int i = 0; i <= divisions; i++) {
        float pos = -gridSize + (i * (gridSize * 2.0f / divisions));
        gridVertices.push_back({glm::vec3(pos, 0.0f, -gridSize), gridColor});
        gridVertices.push_back({glm::vec3(pos, 0.0f, gridSize),  gridColor});
        gridVertices.push_back({glm::vec3(-gridSize, 0.0f, pos), gridColor});
        gridVertices.push_back({glm::vec3(gridSize, 0.0f, pos),  gridColor});
    }
    Mesh grid(gridVertices, GL_LINES);

    Camera camera(glm::vec3(0,0,0), 5.0f);
    
    bool isRunning = true;
    SDL_Event event;
    bool leftDown = false, midDown = false, ctrlHeld = false;
    glm::vec2 selStart(0), selEnd(0);
    static bool showGrid = true;
    static bool pointMode = true; // New Toggle

    while (isRunning) {
        int w, h; SDL_GetWindowSize(window, &w, &h);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)w/(float)h, 0.1f, 100.0f);

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) isRunning = false;
            
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL) ctrlHeld = true;
                if (event.key.keysym.sym == SDLK_z) camera.Reset();
                if (event.key.keysym.sym == SDLK_p) pointMode = !pointMode; // Toggle Point Mode
                if (event.key.keysym.sym == SDLK_s) {
                    glm::vec3 avg(0); int count = 0;
                    for(auto& p : modelPoints) if(p.selected) { avg += p.position; count++; }
                    if(count > 1) {
                        avg /= (float)count;
                        for(auto& p : modelPoints) if(p.selected) p.position = avg;
                    }
                }
            }
            if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL) ctrlHeld = false;
            }

            if (!ImGui::GetIO().WantCaptureMouse) {
                const Uint8* kState = SDL_GetKeyboardState(NULL);
                bool isShiftHeld = kState[SDL_SCANCODE_LSHIFT] || kState[SDL_SCANCODE_RSHIFT];

                if (event.type == SDL_MOUSEWHEEL) camera.ProcessMouseScroll(event.wheel.y);
                
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (event.button.button == SDL_BUTTON_MIDDLE) { 
                        midDown = true; 
                        SDL_SetRelativeMouseMode(SDL_TRUE); 
                    }
                    if (event.button.button == SDL_BUTTON_LEFT) { 
                        leftDown = true; 
                        selStart = glm::vec2(event.button.x, event.button.y); 
                        selEnd = selStart; 
                    }
                }
                
                if (event.type == SDL_MOUSEBUTTONUP) {
                    if (event.button.button == SDL_BUTTON_MIDDLE) { 
                        midDown = false; 
                        SDL_SetRelativeMouseMode(SDL_FALSE); 
                    }
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        leftDown = false;
                        
                        if (!ctrlHeld) {
                            for(auto& p : modelPoints) p.deselect();
                        }

                        float dragDistance = glm::distance(selStart, selEnd);
                        if (dragDistance > 5.0f) {
                            float xMin = std::min(selStart.x, selEnd.x);
                            float xMax = std::max(selStart.x, selEnd.x);
                            float yMin = std::min(selStart.y, selEnd.y);
                            float yMax = std::max(selStart.y, selEnd.y);
                            
                            for(auto& p : modelPoints) {
                                glm::vec2 sp = ProjectToScreen(p.position, view, proj, w, h);
                                if (sp.x >= xMin && sp.x <= xMax && sp.y >= yMin && sp.y <= yMax) {
                                    p.select();
                                }
                            }
                        } else {
                            // Enhanced Selection Logic
                            int bestIdx = -1;
                            float bestDist = 30.0f; // More forgiving radius

                            for (int i = 0; i < (int)modelPoints.size(); i++) {
                                glm::vec2 sp = ProjectToScreen(modelPoints[i].position, view, proj, w, h);
                                float d = glm::distance(selStart, sp);
                                if (d < bestDist) {
                                    bestDist = d;
                                    bestIdx = i;
                                }
                            }
                            if (bestIdx != -1) {
                                modelPoints[bestIdx].select();
                            }
                        }
                    }
                }
                
                if (event.type == SDL_MOUSEMOTION) {
                    if (midDown) {
                        if (isShiftHeld) camera.ProcessMousePan(event.motion.xrel, -event.motion.yrel);
                        else camera.ProcessMouseOrbit(event.motion.xrel, -event.motion.yrel);
                    }
                    if (leftDown) selEnd = glm::vec2(event.motion.x, event.motion.y);
                }
            }
        }

        // Sync data to GPU
        for(int i=0; i < (int)modelPoints.size() && i < (int)myMesh.vertices.size(); i++) {
            myMesh.vertices[i].Position = modelPoints[i].position;
            myMesh.vertices[i].Color = modelPoints[i].color;
        }
        myMesh.updateGPUData();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Export .OBJ")) std::cout << "Exporting model..." << std::endl;
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) isRunning = false;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window")) {
                ImGui::MenuItem("Show Grid", NULL, &showGrid);
                ImGui::MenuItem("Point Mode (P)", NULL, &pointMode);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (leftDown && glm::distance(selStart, selEnd) > 5.0f) {
            ImGui::GetForegroundDrawList()->AddRect(ImVec2(selStart.x, selStart.y), ImVec2(selEnd.x, selEnd.y), IM_COL32(255, 255, 0, 255), 0, 0, 1.5f);
            ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(selStart.x, selStart.y), ImVec2(selEnd.x, selEnd.y), IM_COL32(255, 255, 0, 40));
        }
        ImGui::Render();

        glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProg);
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        if (showGrid) {
            glUniform1i(glGetUniformLocation(shaderProg, "isPoint"), false);
            glUniform1i(glGetUniformLocation(shaderProg, "overrideColor"), false);
            grid.Draw();
        }

        // 1. Draw Mesh Faces
        glUniform1i(glGetUniformLocation(shaderProg, "isPoint"), false);
        glUniform1i(glGetUniformLocation(shaderProg, "overrideColor"), true);
        glUniform3f(glGetUniformLocation(shaderProg, "colorVec"), 0.45f, 0.45f, 0.45f);
        myMesh.drawMode = GL_TRIANGLES;
        myMesh.Draw();

        // 2. Draw Point Handles if Point Mode is on
        if (pointMode) {
            glDisable(GL_DEPTH_TEST); // Ensures handles are ALWAYS visible
            glPointSize(16.0f);
            glUniform1i(glGetUniformLocation(shaderProg, "isPoint"), true);
            glUniform1i(glGetUniformLocation(shaderProg, "overrideColor"), false);
            myMesh.drawMode = GL_POINTS;
            myMesh.Draw();
            glEnable(GL_DEPTH_TEST);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}