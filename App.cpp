#include "App.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

App::App() : window(nullptr), glContext(nullptr), shaderProg(0), 
             camera(nullptr), viewCube(nullptr), myMesh(nullptr), grid(nullptr),
             isRunning(true), leftDown(false), midDown(false), ctrlHeld(false), 
             zHeld(false), draggingPoints(false), isAnimatingCamera(false),
             showGrid(true), pointMode(true), selStart(0), selEnd(0),
             targetYaw(-90.0f), targetPitch(0.0f) {}

App::~App() { cleanup(); }

bool App::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow("Blink3D Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) return false;

    glContext = SDL_GL_CreateContext(window);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    initShaders();
    initGeometry();
    
    camera = new Camera(glm::vec3(0,0,0), 5.0f);
    viewCube = new ViewCube();
    viewCube->init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    return true;
}

void App::initShaders() {
    const char* vSrc = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 ourColor;\n"
        "uniform mat4 model, view, projection;\n"
        "void main() {\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "   ourColor = aColor;\n"
        "   gl_PointSize = 20.0;\n" 
        "}";

    const char* fSrc = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColor;\n"
        "uniform bool isPoint, overrideColor;\n"
        "uniform vec3 colorVec;\n"
        "void main() {\n"
        "   if(isPoint) {\n"
        "       vec3 c = ourColor;\n"
        "       if (length(c) < 0.1) c = vec3(0.0, 1.0, 1.0);\n"
        "       FragColor = vec4(c, 1.0);\n"
        "       return;\n"
        "   }\n"
        "   if(overrideColor) FragColor = vec4(colorVec, 1.0);\n"
        "   else FragColor = vec4(ourColor, 1.0);\n"
        "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs, 1, &vSrc, NULL); glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs, 1, &fSrc, NULL); glCompileShader(fs);
    shaderProg = glCreateProgram(); glAttachShader(shaderProg, vs); glAttachShader(shaderProg, fs); glLinkProgram(shaderProg);
    glDeleteShader(vs); glDeleteShader(fs);
}

void App::initGeometry() {
    modelPoints.clear();
    // Front face
    modelPoints.push_back(Point(glm::vec3(-0.5, -0.5,  0.5))); // 0
    modelPoints.push_back(Point(glm::vec3( 0.5, -0.5,  0.5))); // 1
    modelPoints.push_back(Point(glm::vec3( 0.5,  0.5,  0.5))); // 2
    modelPoints.push_back(Point(glm::vec3(-0.5,  0.5,  0.5))); // 3
    // Back face
    modelPoints.push_back(Point(glm::vec3(-0.5, -0.5, -0.5))); // 4
    modelPoints.push_back(Point(glm::vec3( 0.5, -0.5, -0.5))); // 5
    modelPoints.push_back(Point(glm::vec3( 0.5,  0.5, -0.5))); // 6
    modelPoints.push_back(Point(glm::vec3(-0.5,  0.5, -0.5))); // 7

    std::vector<unsigned int> indices = {
        0,1,2, 2,3,0, // Front
        1,5,6, 6,2,1, // Right
        7,6,5, 5,4,7, // Back
        4,0,3, 3,7,4, // Left
        3,2,6, 6,7,3, // Top
        4,5,1, 1,0,4  // Bottom
    };

    std::vector<Vertex> verts;
    for(auto& p : modelPoints) verts.push_back({p.position, p.color});
    myMesh = new Mesh(verts, GL_TRIANGLES, indices);

    std::vector<Vertex> gv;
    for(int i=-10; i<=10; i++){
        gv.push_back({glm::vec3(i,0,-10), glm::vec3(0.3f)}); gv.push_back({glm::vec3(i,0,10), glm::vec3(0.3f)});
        gv.push_back({glm::vec3(-10,0,i), glm::vec3(0.3f)}); gv.push_back({glm::vec3(10,0,i), glm::vec3(0.3f)});
    }
    grid = new Mesh(gv, GL_LINES);
}

void App::processEvents() {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if(e.type == SDL_QUIT) isRunning = false;

        if(!ImGui::GetIO().WantCaptureMouse) {
            const Uint8* kState = SDL_GetKeyboardState(NULL);
            bool isShift = kState[SDL_SCANCODE_LSHIFT] || kState[SDL_SCANCODE_RSHIFT];
            bool ctrl = kState[SDL_SCANCODE_LCTRL] || kState[SDL_SCANCODE_RCTRL];

            if(e.type == SDL_MOUSEWHEEL) camera->ProcessMouseScroll(e.wheel.y);
            
            if(e.type == SDL_MOUSEBUTTONDOWN) {
                if(e.button.button == SDL_BUTTON_MIDDLE) { 
                    midDown = true; SDL_SetRelativeMouseMode(SDL_TRUE); 
                }
                if(e.button.button == SDL_BUTTON_LEFT) { 
                    int w, h; SDL_GetWindowSize(window, &w, &h);
                    if (viewCube->handleMousePress(e.button.x, e.button.y, w)) continue; // Handled by ViewCube

                    selStart = glm::vec2(e.button.x, e.button.y); 
                    selEnd = selStart; 

                    if (h == 0) h = 1;
                    glm::mat4 v = camera->GetViewMatrix(), p_mat = glm::perspective(glm::radians(45.0f), (float)w/h, 0.1f, 100.0f);
                    
                    int best = -1; float d_min = 40.0f; 
                    for(int i=0; i<(int)modelPoints.size(); i++){
                        glm::vec4 clip = p_mat * v * glm::vec4(modelPoints[i].position, 1.0f);
                        if(clip.w > 0) {
                            glm::vec3 ndc = glm::vec3(clip)/clip.w;
                            float d = glm::distance(selStart, glm::vec2((ndc.x+1)*0.5f*w, (1-ndc.y)*0.5f*h));
                            if(d < d_min) { d_min = d; best = i; }
                        }
                    }

                    if(best != -1) {
                        if (!modelPoints[best].selected) {
                            if (!ctrl) for(auto& p : modelPoints) p.deselect();
                            modelPoints[best].select();
                            draggingPoints = true;
                        } else {
                            if (ctrl) modelPoints[best].deselect();
                            else draggingPoints = true;
                        }
                    } else {
                        leftDown = true; 
                        if(!ctrl) for(auto& p : modelPoints) p.deselect();
                    }
                }
            }
            
            if(e.type == SDL_MOUSEBUTTONUP) {
                if(e.button.button == SDL_BUTTON_MIDDLE) { 
                    midDown = false; SDL_SetRelativeMouseMode(SDL_FALSE); 
                }
                if(e.button.button == SDL_BUTTON_LEFT) {
                    if (viewCube->handleMouseRelease()) continue; // Handled by ViewCube

                    if (draggingPoints) draggingPoints = false;
                    
                    if (leftDown) {
                        leftDown = false;
                        if(glm::distance(selStart, selEnd) > 5.0f) {
                            int w, h; SDL_GetWindowSize(window, &w, &h);
                            if (h == 0) h = 1;
                            glm::mat4 v = camera->GetViewMatrix(), p_mat = glm::perspective(glm::radians(45.0f), (float)w/h, 0.1f, 100.0f);
                            
                            float x1 = std::min(selStart.x, selEnd.x), x2 = std::max(selStart.x, selEnd.x);
                            float y1 = std::min(selStart.y, selEnd.y), y2 = std::max(selStart.y, selEnd.y);
                            for(auto& p_obj : modelPoints) {
                                glm::vec4 clip = p_mat * v * glm::vec4(p_obj.position, 1.0f);
                                if(clip.w > 0) {
                                    glm::vec3 ndc = glm::vec3(clip)/clip.w;
                                    glm::vec2 sp((ndc.x+1)*0.5f*w, (1-ndc.y)*0.5f*h);
                                    if(sp.x >= x1 && sp.x <= x2 && sp.y >= y1 && sp.y <= y2) p_obj.select();
                                }
                            }
                        }
                    }
                }
            }
            
            if(e.type == SDL_MOUSEMOTION) {
                if (viewCube->handleMouseMotion(e.motion.xrel, e.motion.yrel, camera)) {
                    isAnimatingCamera = false; // Cancel animation if viewcube is grabbed
                    targetYaw = camera->Yaw;
                    targetPitch = camera->Pitch;
                    continue;
                }

                if(midDown) {
                    isAnimatingCamera = false; // Cancel animation on manual orbit
                    if(isShift) camera->ProcessMousePan(e.motion.xrel, -e.motion.yrel);
                    else camera->ProcessMouseOrbit(e.motion.xrel, -e.motion.yrel);
                    
                    targetYaw = camera->Yaw;
                    targetPitch = camera->Pitch;
                }
                if(leftDown) selEnd = glm::vec2(e.motion.x, e.motion.y);
                
                if(draggingPoints) {
                    int w, h; SDL_GetWindowSize(window, &w, &h);
                    if (h == 0) h = 1;
                    float unitsPerPixel = (camera->Distance * 0.8284f) / (float)h;
                    glm::vec3 moveDelta = camera->Right * ((float)e.motion.xrel * unitsPerPixel) 
                                        - camera->Up    * ((float)e.motion.yrel * unitsPerPixel);
                    for(auto& p : modelPoints) {
                        if(p.selected) p.position += moveDelta;
                    }
                }
            }
        }
        
        if(e.type == SDL_KEYDOWN) {
            if(e.key.keysym.sym == SDLK_p) pointMode = !pointMode;
            if(e.key.keysym.sym == SDLK_s) {
                glm::vec3 avg(0); int c = 0;
                for(auto& pt : modelPoints) if(pt.selected) { avg += pt.position; c++; }
                if(c > 1) { avg /= (float)c; for(auto& pt : modelPoints) if(pt.selected) pt.position = avg; }
            }
            
            // --- Smoothed Parametric Snapping Logic ---
            if(e.key.keysym.sym == SDLK_z && e.key.repeat == 0) {
                zHeld = true;
                // Snap to the *nearest* 90 degree increment for a smooth transition
                targetYaw = std::round(camera->Yaw / 90.0f) * 90.0f;
                if (camera->Pitch > 45.0f) targetPitch = 89.0f;
                else if (camera->Pitch < -45.0f) targetPitch = -89.0f;
                else targetPitch = 0.0f;
                
                isAnimatingCamera = true;
            }
            
            if(zHeld && e.key.repeat == 0) {
                // Cycle relative to the current target, allowing infinite rotation!
                if(e.key.keysym.sym == SDLK_UP) { targetPitch += 89.0f; isAnimatingCamera = true; }
                if(e.key.keysym.sym == SDLK_DOWN) { targetPitch -= 89.0f; isAnimatingCamera = true; }
                if(e.key.keysym.sym == SDLK_LEFT) { targetYaw -= 90.0f; isAnimatingCamera = true; }
                if(e.key.keysym.sym == SDLK_RIGHT) { targetYaw += 90.0f; isAnimatingCamera = true; }
                
                // Clamp pitch to avoid turning completely upside down
                if (targetPitch > 89.0f) targetPitch = 89.0f;
                if (targetPitch < -89.0f) targetPitch = -89.0f;
            }
        }
        
        if(e.type == SDL_KEYUP) {
            if(e.key.keysym.sym == SDLK_z) zHeld = false;
        }
    }
}

void App::run() {
    while(isRunning) {
        processEvents();
        
        // Start ImGui Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        update();
        buildUI();
        render(); // Rendering pipeline applies ImGui at the very end
    }
}

void App::update() {
    // --- Camera Animation Lerp ---
    if (isAnimatingCamera) {
        float lerpSpeed = 0.15f; // Tweak this decimal for faster/slower snaps
        float newYaw = camera->Yaw + (targetYaw - camera->Yaw) * lerpSpeed;
        float newPitch = camera->Pitch + (targetPitch - camera->Pitch) * lerpSpeed;
        
        camera->SetRotation(newYaw, newPitch);

        // Snap perfectly and stop calculating if we are close enough
        if (std::abs(targetYaw - camera->Yaw) < 0.1f && std::abs(targetPitch - camera->Pitch) < 0.1f) {
            camera->SetRotation(targetYaw, targetPitch);
            isAnimatingCamera = false;
        }
    }

    for(int i=0; i<(int)modelPoints.size(); i++) {
        myMesh->vertices[i].Position = modelPoints[i].position;
        myMesh->vertices[i].Color = modelPoints[i].color;
    }
    myMesh->updateGPUData();
}

void App::buildUI() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) isRunning = false;
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
}

void App::render() {
    int winW, winH, drawW, drawH; 
    SDL_GetWindowSize(window, &winW, &winH);
    SDL_GL_GetDrawableSize(window, &drawW, &drawH);
    if (winH == 0) winH = 1;

    // --- 1. Draw Main Scene ---
    glViewport(0, 0, drawW, drawH);
    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 v = camera->GetViewMatrix(), p = glm::perspective(glm::radians(45.0f), (float)winW/winH, 0.1f, 100.0f);

    glUseProgram(shaderProg);
    glUniformMatrix4fv(glGetUniformLocation(shaderProg, "view"), 1, GL_FALSE, glm::value_ptr(v));
    glUniformMatrix4fv(glGetUniformLocation(shaderProg, "projection"), 1, GL_FALSE, glm::value_ptr(p));
    glUniformMatrix4fv(glGetUniformLocation(shaderProg, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    if(showGrid) {
        glUniform1i(glGetUniformLocation(shaderProg, "isPoint"), false);
        glUniform1i(glGetUniformLocation(shaderProg, "overrideColor"), false);
        grid->Draw();
    }

    // Draw Solid Mesh
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glUniform1i(glGetUniformLocation(shaderProg, "isPoint"), false);
    glUniform1i(glGetUniformLocation(shaderProg, "overrideColor"), true);
    glUniform3f(glGetUniformLocation(shaderProg, "colorVec"), 0.45f, 0.45f, 0.45f);
    myMesh->drawMode = GL_TRIANGLES;
    myMesh->Draw();

    // Draw Wireframe Edges
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0f, -1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUniform3f(glGetUniformLocation(shaderProg, "colorVec"), 0.1f, 0.1f, 0.1f);
    myMesh->Draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);

    // Draw Points
    if(pointMode) {
        glDisable(GL_DEPTH_TEST); 
        glUniform1i(glGetUniformLocation(shaderProg, "isPoint"), true);
        glUniform1i(glGetUniformLocation(shaderProg, "overrideColor"), false);
        
        glBindVertexArray(myMesh->VAO);
        glDrawArrays(GL_POINTS, 0, myMesh->vertices.size());
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
    }

    // --- 2. Draw ViewCube Overlay ---
    viewCube->draw(camera, winW, winH, drawW, drawH, shaderProg);

    // --- 3. Render ImGui pass (Menu bar, Selection Box, ViewCube text) ---
    ImGui::Render(); 
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);
}

void App::cleanup() {
    ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplSDL2_Shutdown(); ImGui::DestroyContext();
    if(glContext) SDL_GL_DeleteContext(glContext);
    if(window) SDL_DestroyWindow(window);
    SDL_Quit();
}