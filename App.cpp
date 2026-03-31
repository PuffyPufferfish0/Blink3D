#include "App.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

App::App() : window(nullptr), glContext(nullptr), shaderProg(0), 
             camera(nullptr), viewCube(nullptr), toolbar(nullptr), gizmo(nullptr),
             myMesh(nullptr), grid(nullptr),
             isRunning(true), leftDown(false), midDown(false), ctrlHeld(false), 
             isAnimatingCamera(false), draggingPoints(false),
             showGrid(true), pointMode(true), showPreferencesWindow(false), selStart(0), selEnd(0),
             targetYaw(-90.0f), targetPitch(0.0f),
             historyIndex(-1), hasUnsavedChanges(false) {}

App::~App() { cleanup(); }

void App::loadConfig() {
    std::ifstream file(".configSettings");
    if (file.is_open()) {
        std::string key;
        while (file >> key) {
            if (key == "exportDir") file >> config.exportDirectory;
            else if (key == "bgColor") file >> config.bgColor.r >> config.bgColor.g >> config.bgColor.b;
            else if (key == "unselPoint") file >> config.unselectedPointColor.r >> config.unselectedPointColor.g >> config.unselectedPointColor.b;
            else if (key == "selPoint") file >> config.selectedPointColor.r >> config.selectedPointColor.g >> config.selectedPointColor.b;
            else if (key == "unselLine") file >> config.unselectedLineColor.r >> config.unselectedLineColor.g >> config.unselectedLineColor.b;
            else if (key == "selLine") file >> config.selectedLineColor.r >> config.selectedLineColor.g >> config.selectedLineColor.b;
            else if (key == "unselFace") file >> config.unselectedFaceColor.r >> config.unselectedFaceColor.g >> config.unselectedFaceColor.b;
            else if (key == "selFace") file >> config.selectedFaceColor.r >> config.selectedFaceColor.g >> config.selectedFaceColor.b;
            else if (key == "pointSize") file >> config.pointSize;
            else if (key == "lineThick") file >> config.lineThickness;
            else if (key == "winRes") file >> config.windowResIndex;
        }
        file.close();
    }
}

void App::saveConfig() {
    std::ofstream file(".configSettings");
    if (file.is_open()) {
        file << "exportDir " << config.exportDirectory << "\n";
        file << "bgColor " << config.bgColor.r << " " << config.bgColor.g << " " << config.bgColor.b << "\n";
        file << "unselPoint " << config.unselectedPointColor.r << " " << config.unselectedPointColor.g << " " << config.unselectedPointColor.b << "\n";
        file << "selPoint " << config.selectedPointColor.r << " " << config.selectedPointColor.g << " " << config.selectedPointColor.b << "\n";
        file << "unselLine " << config.unselectedLineColor.r << " " << config.unselectedLineColor.g << " " << config.unselectedLineColor.b << "\n";
        file << "selLine " << config.selectedLineColor.r << " " << config.selectedLineColor.g << " " << config.selectedLineColor.b << "\n";
        file << "unselFace " << config.unselectedFaceColor.r << " " << config.unselectedFaceColor.g << " " << config.unselectedFaceColor.b << "\n";
        file << "selFace " << config.selectedFaceColor.r << " " << config.selectedFaceColor.g << " " << config.selectedFaceColor.b << "\n";
        file << "pointSize " << config.pointSize << "\n";
        file << "lineThick " << config.lineThickness << "\n";
        file << "winRes " << config.windowResIndex << "\n";
        file.close();
    }
}

void App::applyWindowResolution() {
    if (!window) return;
    int w = 1280, h = 720;
    if(config.windowResIndex == 0) { w=800; h=600; }
    else if(config.windowResIndex == 1) { w=1280; h=720; }
    else if(config.windowResIndex == 2) { w=1920; h=1080; }
    else if(config.windowResIndex == 3) { w=2560; h=1440; }
    SDL_SetWindowSize(window, w, h);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

bool App::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    loadConfig();

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    int startW = 1280, startH = 720;
    if(config.windowResIndex == 0) { startW=800; startH=600; }
    else if(config.windowResIndex == 1) { startW=1280; startH=720; }
    else if(config.windowResIndex == 2) { startW=1920; startH=1080; }
    else if(config.windowResIndex == 3) { startW=2560; startH=1440; }

    window = SDL_CreateWindow("Blink3D Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, startW, startH, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) return false;

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) return false;

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(nullptr);

    initShaders();
    initGeometry();
    
    camera = new Camera(glm::vec3(0,0,0), 5.0f);
    viewCube = new ViewCube();
    viewCube->init();
    
    toolbar = new Toolbar();
    gizmo = new TransformGizmo();
    gizmo->init();

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
        "uniform float pointSize;\n"
        "void main() {\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "   ourColor = aColor;\n"
        "   gl_PointSize = pointSize;\n" 
        "}";

    const char* fSrc = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColor;\n"
        "uniform bool isPoint, overrideColor;\n"
        "uniform vec3 colorVec;\n"
        "void main() {\n"
        "   if(isPoint) {\n"
        "       FragColor = vec4(ourColor, 1.0);\n"
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
    modelPoints.push_back(Point(glm::vec3(-0.5, -0.5,  0.5))); 
    modelPoints.push_back(Point(glm::vec3( 0.5, -0.5,  0.5))); 
    modelPoints.push_back(Point(glm::vec3( 0.5,  0.5,  0.5))); 
    modelPoints.push_back(Point(glm::vec3(-0.5,  0.5,  0.5))); 
    modelPoints.push_back(Point(glm::vec3(-0.5, -0.5, -0.5))); 
    modelPoints.push_back(Point(glm::vec3( 0.5, -0.5, -0.5))); 
    modelPoints.push_back(Point(glm::vec3( 0.5,  0.5, -0.5))); 
    modelPoints.push_back(Point(glm::vec3(-0.5,  0.5, -0.5))); 

    std::vector<unsigned int> indices = {
        0,1,2, 2,3,0, 1,5,6, 6,2,1, 7,6,5, 5,4,7, 4,0,3, 3,7,4, 3,2,6, 6,7,3, 4,5,1, 1,0,4 
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

    extractTopologyFromMesh();

    history.clear();
    historyIndex = -1;
    saveState();
}

void App::extractTopologyFromMesh() {
    modelLines.clear();
    modelFaces.clear();
    for (size_t i = 0; i < myMesh->indices.size(); i += 3) {
        Line l1(myMesh->indices[i], myMesh->indices[i+1]);
        Line l2(myMesh->indices[i+1], myMesh->indices[i+2]);
        Line l3(myMesh->indices[i+2], myMesh->indices[i]);
        
        if (std::find(modelLines.begin(), modelLines.end(), l1) == modelLines.end()) modelLines.push_back(l1);
        if (std::find(modelLines.begin(), modelLines.end(), l2) == modelLines.end()) modelLines.push_back(l2);
        if (std::find(modelLines.begin(), modelLines.end(), l3) == modelLines.end()) modelLines.push_back(l3);

        modelFaces.push_back(Face(myMesh->indices[i], myMesh->indices[i+1], myMesh->indices[i+2]));
    }
}

void App::saveState() {
    if (historyIndex < (int)history.size() - 1) {
        history.erase(history.begin() + historyIndex + 1, history.end());
    }
    history.push_back({modelPoints, myMesh->indices, modelLines, modelFaces});
    historyIndex++;
}

void App::undo() {
    if (historyIndex > 0) {
        historyIndex--;
        modelPoints = history[historyIndex].points;
        modelLines = history[historyIndex].lines;
        modelFaces = history[historyIndex].faces;
        
        std::vector<Vertex> verts;
        for(auto& p : modelPoints) verts.push_back({p.position, p.color});
        delete myMesh;
        myMesh = new Mesh(verts, GL_TRIANGLES, history[historyIndex].indices);
        
        selectedIndices.clear();
        for (int i = 0; i < (int)modelPoints.size(); i++) {
            if (modelPoints[i].selected) selectedIndices.push_back(i);
        }
        gizmo->updateState(modelPoints, modelLines, modelFaces);
    }
}

void App::redo() {
    if (historyIndex < (int)history.size() - 1) {
        historyIndex++;
        modelPoints = history[historyIndex].points;
        modelLines = history[historyIndex].lines;
        modelFaces = history[historyIndex].faces;
        
        std::vector<Vertex> verts;
        for(auto& p : modelPoints) verts.push_back({p.position, p.color});
        delete myMesh;
        myMesh = new Mesh(verts, GL_TRIANGLES, history[historyIndex].indices);
        
        selectedIndices.clear();
        for (int i = 0; i < (int)modelPoints.size(); i++) {
            if (modelPoints[i].selected) selectedIndices.push_back(i);
        }
        gizmo->updateState(modelPoints, modelLines, modelFaces);
    }
}

void App::processEvents() {
    SDL_Event e;
    
    auto distToSegment = [](glm::vec2 p, glm::vec2 v, glm::vec2 w) {
        float l2 = glm::distance(v, w) * glm::distance(v, w);
        if (l2 == 0.0f) return glm::distance(p, v);
        float t = std::max(0.0f, std::min(1.0f, glm::dot(p - v, w - v) / l2));
        glm::vec2 projection = v + t * (w - v);
        return glm::distance(p, projection);
    };

    auto pointInTriangle = [](glm::vec2 pt, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3) {
        auto sign = [](glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
            return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
        };
        float d1 = sign(pt, v1, v2);
        float d2 = sign(pt, v2, v3);
        float d3 = sign(pt, v3, v1);
        bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
        return !(has_neg && has_pos);
    };
    
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
                    if (h == 0) h = 1;
                    
                    if (viewCube->handleMousePress(e.button.x, e.button.y, w)) continue; 

                    if (toolbar->currentTool == ToolMode::MOVE && gizmo->handleMousePress(e.button.x, e.button.y, camera, w, h)) {
                        continue; 
                    }

                    selStart = glm::vec2(e.button.x, e.button.y); 
                    selEnd = selStart; 

                    glm::mat4 v = camera->GetViewMatrix(), p_mat = glm::perspective(glm::radians(45.0f), (float)w/h, 0.1f, 100.0f);
                    
                    if (toolbar->selectMode == SelectMode::FACE) {
                        int best = -1; float z_min = 1.0f;
                        for(int i=0; i<(int)modelFaces.size(); i++){
                            glm::vec4 c1 = p_mat * v * glm::vec4(modelPoints[modelFaces[i].v1].position, 1.0f);
                            glm::vec4 c2 = p_mat * v * glm::vec4(modelPoints[modelFaces[i].v2].position, 1.0f);
                            glm::vec4 c3 = p_mat * v * glm::vec4(modelPoints[modelFaces[i].v3].position, 1.0f);
                            if(c1.w > 0 && c2.w > 0 && c3.w > 0) {
                                glm::vec2 s1((c1.x/c1.w+1)*0.5f*w, (1-c1.y/c1.w)*0.5f*h);
                                glm::vec2 s2((c2.x/c2.w+1)*0.5f*w, (1-c2.y/c2.w)*0.5f*h);
                                glm::vec2 s3((c3.x/c3.w+1)*0.5f*w, (1-c3.y/c3.w)*0.5f*h);
                                if (pointInTriangle(selStart, s1, s2, s3)) {
                                    float avgZ = (c1.z/c1.w + c2.z/c2.w + c3.z/c3.w) / 3.0f;
                                    if (avgZ < z_min) { z_min = avgZ; best = i; }
                                }
                            }
                        }

                        if(best != -1) {
                            if (!modelFaces[best].selected) {
                                if (!ctrl) for(auto& f : modelFaces) f.deselect();
                                modelFaces[best].select();
                                saveState();
                                draggingPoints = true;
                            } else {
                                if (ctrl) { modelFaces[best].deselect(); saveState(); }
                                else draggingPoints = true;
                            }
                        } else {
                            leftDown = true; 
                            bool changed = false;
                            if(!ctrl) {
                                for(auto& f : modelFaces) { if (f.selected) changed = true; f.deselect(); }
                            }
                            if (changed) saveState(); 
                        }
                    } else if (toolbar->selectMode == SelectMode::LINE) {
                        int best = -1; float d_min = 15.0f; 
                        for(int i=0; i<(int)modelLines.size(); i++){
                            glm::vec4 c1 = p_mat * v * glm::vec4(modelPoints[modelLines[i].v1].position, 1.0f);
                            glm::vec4 c2 = p_mat * v * glm::vec4(modelPoints[modelLines[i].v2].position, 1.0f);
                            if(c1.w > 0 && c2.w > 0) {
                                glm::vec2 s1((c1.x/c1.w+1)*0.5f*w, (1-c1.y/c1.w)*0.5f*h);
                                glm::vec2 s2((c2.x/c2.w+1)*0.5f*w, (1-c2.y/c2.w)*0.5f*h);
                                float d = distToSegment(selStart, s1, s2);
                                if(d < d_min) { d_min = d; best = i; }
                            }
                        }

                        if(best != -1) {
                            if (!modelLines[best].selected) {
                                if (!ctrl) for(auto& l : modelLines) l.deselect();
                                modelLines[best].select();
                                saveState();
                                draggingPoints = true;
                            } else {
                                if (ctrl) { modelLines[best].deselect(); saveState(); }
                                else draggingPoints = true;
                            }
                        } else {
                            leftDown = true; 
                            bool changed = false;
                            if(!ctrl) {
                                for(auto& l : modelLines) { if (l.selected) changed = true; l.deselect(); }
                            }
                            if (changed) saveState(); 
                        }
                    } else { // Standard Point Selection
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
                                if (!ctrl) {
                                    for(auto& p : modelPoints) p.deselect();
                                    selectedIndices.clear();
                                }
                                modelPoints[best].select();
                                selectedIndices.push_back(best);
                                saveState(); 
                                draggingPoints = true; 
                            } else {
                                if (ctrl) {
                                    modelPoints[best].deselect();
                                    selectedIndices.erase(std::remove(selectedIndices.begin(), selectedIndices.end(), best), selectedIndices.end());
                                    saveState(); 
                                }
                                else draggingPoints = true; 
                            }
                        } else {
                            leftDown = true; 
                            bool changed = false;
                            if(!ctrl) {
                                for(auto& p : modelPoints) { if (p.selected) changed = true; p.deselect(); }
                                selectedIndices.clear();
                            }
                            if (changed) saveState(); 
                        }
                    }
                }
            }
            
            if(e.type == SDL_MOUSEBUTTONUP) {
                if(e.button.button == SDL_BUTTON_MIDDLE) { 
                    midDown = false; SDL_SetRelativeMouseMode(SDL_FALSE); 
                }
                if(e.button.button == SDL_BUTTON_LEFT) {
                    if (viewCube->handleMouseRelease()) continue; 
                    
                    if (gizmo->handleMouseRelease()) {
                        if (hasUnsavedChanges) { saveState(); hasUnsavedChanges = false; }
                        continue; 
                    }
                    
                    if (draggingPoints) {
                        draggingPoints = false;
                        if (hasUnsavedChanges) { saveState(); hasUnsavedChanges = false; }
                    }
                    
                    if (leftDown) {
                        leftDown = false;
                        if(glm::distance(selStart, selEnd) > 5.0f) {
                            int w, h; SDL_GetWindowSize(window, &w, &h);
                            if (h == 0) h = 1;
                            glm::mat4 v = camera->GetViewMatrix(), p_mat = glm::perspective(glm::radians(45.0f), (float)w/h, 0.1f, 100.0f);
                            
                            float x1 = std::min(selStart.x, selEnd.x), x2 = std::max(selStart.x, selEnd.x);
                            float y1 = std::min(selStart.y, selEnd.y), y2 = std::max(selStart.y, selEnd.y);
                            bool changed = false;
                            
                            if (toolbar->selectMode == SelectMode::POINT) {
                                for(int i = 0; i < (int)modelPoints.size(); i++) {
                                    auto& p_obj = modelPoints[i];
                                    glm::vec4 clip = p_mat * v * glm::vec4(p_obj.position, 1.0f);
                                    if(clip.w > 0) {
                                        glm::vec3 ndc = glm::vec3(clip)/clip.w;
                                        glm::vec2 sp((ndc.x+1)*0.5f*w, (1-ndc.y)*0.5f*h);
                                        if(sp.x >= x1 && sp.x <= x2 && sp.y >= y1 && sp.y <= y2) {
                                            if (!p_obj.selected) {
                                                changed = true;
                                                p_obj.select();
                                                selectedIndices.push_back(i);
                                            }
                                        }
                                    }
                                }
                            } else if (toolbar->selectMode == SelectMode::FACE) {
                                for(int i = 0; i < (int)modelFaces.size(); i++) {
                                    auto& f_obj = modelFaces[i];
                                    glm::vec4 c1 = p_mat * v * glm::vec4(modelPoints[f_obj.v1].position, 1.0f);
                                    glm::vec4 c2 = p_mat * v * glm::vec4(modelPoints[f_obj.v2].position, 1.0f);
                                    glm::vec4 c3 = p_mat * v * glm::vec4(modelPoints[f_obj.v3].position, 1.0f);
                                    if(c1.w > 0 && c2.w > 0 && c3.w > 0) {
                                        glm::vec2 s1((c1.x/c1.w+1)*0.5f*w, (1-c1.y/c1.w)*0.5f*h);
                                        glm::vec2 s2((c2.x/c2.w+1)*0.5f*w, (1-c2.y/c2.w)*0.5f*h);
                                        glm::vec2 s3((c3.x/c3.w+1)*0.5f*w, (1-c3.y/c3.w)*0.5f*h);
                                        if(s1.x >= x1 && s1.x <= x2 && s1.y >= y1 && s1.y <= y2 &&
                                           s2.x >= x1 && s2.x <= x2 && s2.y >= y1 && s2.y <= y2 &&
                                           s3.x >= x1 && s3.x <= x2 && s3.y >= y1 && s3.y <= y2) {
                                            if (!f_obj.selected) { changed = true; f_obj.select(); }
                                        }
                                    }
                                }
                            }
                            if (changed) saveState(); 
                        }
                    }
                }
            }
            
            if(e.type == SDL_MOUSEMOTION) {
                if (viewCube->handleMouseMotion(e.motion.xrel, e.motion.yrel, camera)) {
                    isAnimatingCamera = false;
                    targetYaw = camera->Yaw;
                    targetPitch = camera->Pitch;
                    continue;
                }
                
                int w, h; SDL_GetWindowSize(window, &w, &h);
                if (h == 0) h = 1;
                
                if (gizmo->handleMouseMotion(e.motion.xrel, e.motion.yrel, camera, w, h, modelPoints, modelLines, modelFaces)) {
                    hasUnsavedChanges = true; 
                    continue; 
                }

                if (draggingPoints) {
                    hasUnsavedChanges = true; 
                    float unitsPerPixel = (camera->Distance * 0.8284f) / (float)h;
                    glm::vec3 moveDelta = camera->Right * ((float)e.motion.xrel * unitsPerPixel) 
                                        - camera->Up    * ((float)e.motion.yrel * unitsPerPixel);
                                        
                    std::vector<bool> moveFlag(modelPoints.size(), false);
                    for (size_t i = 0; i < modelPoints.size(); i++) if (modelPoints[i].selected) moveFlag[i] = true;
                    for (const auto& l : modelLines) if (l.selected) { moveFlag[l.v1] = true; moveFlag[l.v2] = true; }
                    for (const auto& f : modelFaces) if (f.selected) { moveFlag[f.v1] = true; moveFlag[f.v2] = true; moveFlag[f.v3] = true; }
                    
                    for (size_t i = 0; i < modelPoints.size(); i++) {
                        if (moveFlag[i]) modelPoints[i].position += moveDelta;
                    }
                }

                if(midDown) {
                    isAnimatingCamera = false;
                    if(isShift) camera->ProcessMousePan(e.motion.xrel, -e.motion.yrel);
                    else camera->ProcessMouseOrbit(e.motion.xrel, -e.motion.yrel);
                    
                    targetYaw = camera->Yaw;
                    targetPitch = camera->Pitch;
                }
                if(leftDown) selEnd = glm::vec2(e.motion.x, e.motion.y);
            }
        } 
        
        if(e.type == SDL_KEYDOWN) {
            const Uint8* kState = SDL_GetKeyboardState(NULL);
            bool ctrl = kState[SDL_SCANCODE_LCTRL] || kState[SDL_SCANCODE_RCTRL];
            
            if(e.key.keysym.sym == SDLK_p) pointMode = !pointMode;
            if(e.key.keysym.sym == SDLK_g && !ctrl) showGrid = !showGrid;
            
            if(e.key.keysym.sym == SDLK_1) toolbar->selectMode = SelectMode::POINT;
            if(e.key.keysym.sym == SDLK_2) toolbar->selectMode = SelectMode::LINE;
            if(e.key.keysym.sym == SDLK_3) toolbar->selectMode = SelectMode::FACE; 
            
            if(e.key.keysym.sym == SDLK_t) toolbar->currentTool = ToolMode::MOVE;
            if(e.key.keysym.sym == SDLK_r) toolbar->currentTool = ToolMode::ROTATE;
            if(e.key.keysym.sym == SDLK_e) toolbar->currentTool = ToolMode::SCALE;
            
            if(e.key.keysym.sym == SDLK_q) {
                if (SplitTool::execute(modelPoints, myMesh->indices, modelLines)) {
                    std::vector<Vertex> verts;
                    for(auto& p : modelPoints) verts.push_back({p.position, p.color});
                    std::vector<unsigned int> newInds = myMesh->indices;
                    delete myMesh;
                    myMesh = new Mesh(verts, GL_TRIANGLES, newInds);
                    
                    extractTopologyFromMesh();
                    for(auto& l : modelLines) l.deselect();
                    for(auto& f : modelFaces) f.deselect();
                    for(auto& p : modelPoints) p.deselect();
                    selectedIndices.clear();
                    
                    gizmo->updateState(modelPoints, modelLines, modelFaces);
                    saveState(); 
                }
            }
            
            if(e.key.keysym.sym == SDLK_s) {
                if(selectedIndices.size() >= 2) {
                    int sourceIdx = selectedIndices[0];
                    int targetIdx = selectedIndices[1];
                    
                    for (size_t i = 0; i < myMesh->indices.size(); i++) {
                        if (myMesh->indices[i] == (unsigned int)sourceIdx) {
                            myMesh->indices[i] = targetIdx;
                        }
                    }
                    
                    modelPoints.erase(modelPoints.begin() + sourceIdx);
                    
                    for (size_t i = 0; i < myMesh->indices.size(); i++) {
                        if (myMesh->indices[i] > (unsigned int)sourceIdx) {
                            myMesh->indices[i]--;
                        }
                    }
                    
                    std::vector<Vertex> verts;
                    for(auto& p : modelPoints) verts.push_back({p.position, p.color});
                    std::vector<unsigned int> newIndices = myMesh->indices;
                    delete myMesh;
                    myMesh = new Mesh(verts, GL_TRIANGLES, newIndices);
                    
                    for(auto& pt : modelPoints) pt.deselect();
                    selectedIndices.clear();
                    
                    extractTopologyFromMesh();
                    gizmo->updateState(modelPoints, modelLines, modelFaces);
                    
                    saveState(); 
                }
            }
            
            if (ctrl && e.key.keysym.sym == SDLK_z && e.key.repeat == 0) {
                undo();
            } else if (ctrl && e.key.keysym.sym == SDLK_y && e.key.repeat == 0) {
                redo();
            }
            
            if(!ctrl && e.key.keysym.sym == SDLK_z && e.key.repeat == 0) {
                targetYaw = std::round(camera->Yaw / 90.0f) * 90.0f;
                targetPitch = std::round(camera->Pitch / 90.0f) * 90.0f;
                isAnimatingCamera = true;
            }
            
            if(kState[SDL_SCANCODE_Z] && e.key.repeat == 0 && !ctrl) {
                if(e.key.keysym.sym == SDLK_UP) { targetPitch += 90.0f; isAnimatingCamera = true; }
                if(e.key.keysym.sym == SDLK_DOWN) { targetPitch -= 90.0f; isAnimatingCamera = true; }
                if(e.key.keysym.sym == SDLK_LEFT) { targetYaw -= 90.0f; isAnimatingCamera = true; }
                if(e.key.keysym.sym == SDLK_RIGHT) { targetYaw += 90.0f; isAnimatingCamera = true; }
                
                if (targetPitch > 90.1f) {
                    targetPitch -= 180.0f;
                    targetYaw += 180.0f;
                } else if (targetPitch < -90.1f) {
                    targetPitch += 180.0f;
                    targetYaw += 180.0f;
                }
            }
        }
    }
}

void App::run() {
    while(isRunning) {
        processEvents();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        update();
        buildUI();
        render(); 
    }
}

void App::update() {
    if (isAnimatingCamera) {
        float lerpSpeed = 0.15f; 
        
        float effectiveTargetPitch = targetPitch;
        if (effectiveTargetPitch > 89.0f) effectiveTargetPitch = 89.0f;
        if (effectiveTargetPitch < -89.0f) effectiveTargetPitch = -89.0f;

        float newYaw = camera->Yaw + (targetYaw - camera->Yaw) * lerpSpeed;
        float newPitch = camera->Pitch + (effectiveTargetPitch - camera->Pitch) * lerpSpeed;
        
        camera->SetRotation(newYaw, newPitch);

        if (std::abs(targetYaw - camera->Yaw) < 0.1f && std::abs(effectiveTargetPitch - camera->Pitch) < 0.1f) {
            camera->SetRotation(targetYaw, effectiveTargetPitch);
            isAnimatingCamera = false;
        }
    }

    for(int i=0; i<(int)modelPoints.size(); i++) {
        myMesh->vertices[i].Position = modelPoints[i].position;
        // Inject configuration colors here! Overrides default point colors.
        myMesh->vertices[i].Color = modelPoints[i].selected ? config.selectedPointColor : config.unselectedPointColor;
    }
    myMesh->updateGPUData();
    
    gizmo->updateState(modelPoints, modelLines, modelFaces);
}

void App::buildUI() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) isRunning = false;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Preferences")) showPreferencesWindow = true; 
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Show Grid (G)", NULL, &showGrid); 
            ImGui::MenuItem("Point Mode (P)", NULL, &pointMode);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (showPreferencesWindow) {
        ImGui::Begin("Preferences", &showPreferencesWindow);
        ImGui::InputText("Main Export Directory", config.exportDirectory, IM_ARRAYSIZE(config.exportDirectory));
        
        if (ImGui::CollapsingHeader("Theme", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::ColorEdit3("Background Color", glm::value_ptr(config.bgColor));
            ImGui::ColorEdit3("Unselected Point Color", glm::value_ptr(config.unselectedPointColor));
            ImGui::ColorEdit3("Selected Point Color", glm::value_ptr(config.selectedPointColor));
            ImGui::ColorEdit3("Unselected Line Color", glm::value_ptr(config.unselectedLineColor));
            ImGui::ColorEdit3("Selected Line Color", glm::value_ptr(config.selectedLineColor));
            ImGui::ColorEdit3("Unselected Face Color", glm::value_ptr(config.unselectedFaceColor));
            ImGui::ColorEdit3("Selected Face Color", glm::value_ptr(config.selectedFaceColor));
        }

        ImGui::SliderFloat("Point Size", &config.pointSize, 5.0f, 50.0f, "%.1f px");
        ImGui::SliderFloat("Line Thickness", &config.lineThickness, 1.0f, 10.0f, "%.1f px");

        const char* resOptions[] = { "800x600", "1280x720", "1920x1080", "2560x1440" };
        if (ImGui::Combo("Default Window Resolution", &config.windowResIndex, resOptions, IM_ARRAYSIZE(resOptions))) {
            applyWindowResolution();
            saveConfig(); // Save instantly when changed
        }

        ImGui::Dummy(ImVec2(0, 10));
        
        if (ImGui::Button("Save to config file", ImVec2(160, 30))) {
            saveConfig();
        }
        ImGui::SameLine();
        if (ImGui::Button(".configSettings file export", ImVec2(220, 30))) {
            // Future file-browser trigger goes here. For now, it forces a save to disk!
            saveConfig();
        }

        ImGui::End();
    }

    if (leftDown && glm::distance(selStart, selEnd) > 5.0f) {
        ImGui::GetForegroundDrawList()->AddRect(ImVec2(selStart.x, selStart.y), ImVec2(selEnd.x, selEnd.y), IM_COL32(255, 255, 0, 255), 0, 0, 1.5f);
        ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(selStart.x, selStart.y), ImVec2(selEnd.x, selEnd.y), IM_COL32(255, 255, 0, 40));
    }
    
    int winH; SDL_GetWindowSize(window, nullptr, &winH);
    toolbar->draw(winH);
}

void App::render() {
    int winW, winH, drawW, drawH; 
    SDL_GetWindowSize(window, &winW, &winH);
    SDL_GL_GetDrawableSize(window, &drawW, &drawH);
    if (winH == 0) winH = 1;

    glViewport(0, 0, drawW, drawH);
    // Dynamic config background color
    glClearColor(config.bgColor.r, config.bgColor.g, config.bgColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 v = camera->GetViewMatrix(), p = glm::perspective(glm::radians(45.0f), (float)winW/winH, 0.1f, 100.0f);

    glUseProgram(shaderProg);
    glUniformMatrix4fv(glGetUniformLocation(shaderProg, "view"), 1, GL_FALSE, glm::value_ptr(v));
    glUniformMatrix4fv(glGetUniformLocation(shaderProg, "projection"), 1, GL_FALSE, glm::value_ptr(p));
    glUniformMatrix4fv(glGetUniformLocation(shaderProg, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniform1f(glGetUniformLocation(shaderProg, "pointSize"), config.pointSize);

    if(showGrid) {
        glUniform1i(glGetUniformLocation(shaderProg, "isPoint"), false);
        glUniform1i(glGetUniformLocation(shaderProg, "overrideColor"), false);
        grid->Draw();
    }

    // Dynamic config unselected face color
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glUniform1i(glGetUniformLocation(shaderProg, "isPoint"), false);
    glUniform1i(glGetUniformLocation(shaderProg, "overrideColor"), true);
    glUniform3f(glGetUniformLocation(shaderProg, "colorVec"), config.unselectedFaceColor.r, config.unselectedFaceColor.g, config.unselectedFaceColor.b);
    myMesh->drawMode = GL_TRIANGLES;
    myMesh->Draw();

    // Dynamic config unselected line color
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0f, -1.0f);
    glLineWidth(config.lineThickness);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUniform3f(glGetUniformLocation(shaderProg, "colorVec"), config.unselectedLineColor.r, config.unselectedLineColor.g, config.unselectedLineColor.b);
    myMesh->Draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    
    // Dynamic config selected line color
    std::vector<Vertex> selLineVerts;
    for (const auto& l : modelLines) {
        if (l.selected) {
            selLineVerts.push_back({modelPoints[l.v1].position, config.selectedLineColor});
            selLineVerts.push_back({modelPoints[l.v2].position, config.selectedLineColor});
        }
    }
    if (!selLineVerts.empty()) {
        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, selLineVerts.size() * sizeof(Vertex), selLineVerts.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
        
        glDisable(GL_DEPTH_TEST);
        glUseProgram(shaderProg);
        glUniform1i(glGetUniformLocation(shaderProg, "isPoint"), false);
        glUniform1i(glGetUniformLocation(shaderProg, "overrideColor"), false);
        glDrawArrays(GL_LINES, 0, selLineVerts.size());
        
        glEnable(GL_DEPTH_TEST);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
    
    // Dynamic config selected face color
    std::vector<Vertex> selFaceVerts;
    for (const auto& f : modelFaces) {
        if (f.selected) {
            selFaceVerts.push_back({modelPoints[f.v1].position, config.selectedFaceColor});
            selFaceVerts.push_back({modelPoints[f.v2].position, config.selectedFaceColor});
            selFaceVerts.push_back({modelPoints[f.v3].position, config.selectedFaceColor});
        }
    }
    if (!selFaceVerts.empty()) {
        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, selFaceVerts.size() * sizeof(Vertex), selFaceVerts.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
        
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-2.0f, -2.0f); 
        
        glUseProgram(shaderProg);
        glUniform1i(glGetUniformLocation(shaderProg, "isPoint"), false);
        glUniform1i(glGetUniformLocation(shaderProg, "overrideColor"), false);
        glDrawArrays(GL_TRIANGLES, 0, selFaceVerts.size());
        
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    if(pointMode) {
        glDisable(GL_DEPTH_TEST); 
        glUniform1i(glGetUniformLocation(shaderProg, "isPoint"), true);
        glUniform1i(glGetUniformLocation(shaderProg, "overrideColor"), false);
        
        glBindVertexArray(myMesh->VAO);
        glDrawArrays(GL_POINTS, 0, myMesh->vertices.size());
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
    }

    if (toolbar->currentTool == ToolMode::MOVE) {
        gizmo->draw(camera, winW, winH, shaderProg);
    }

    viewCube->draw(camera, winW, winH, drawW, drawH, shaderProg);

    ImGui::Render(); 
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);
}

void App::cleanup() {
    if (ImGui::GetCurrentContext()) {
        if (ImGui::GetIO().BackendRendererUserData != nullptr) {
            ImGui_ImplOpenGL3_Shutdown();
        }
        if (ImGui::GetIO().BackendPlatformUserData != nullptr) {
            ImGui_ImplSDL2_Shutdown();
        }
        ImGui::DestroyContext();
    }
    
    delete toolbar;
    delete gizmo;
    
    if(glContext) { SDL_GL_DeleteContext(glContext); glContext = nullptr; }
    if(window) { SDL_DestroyWindow(window); window = nullptr; }
    SDL_Quit();
}