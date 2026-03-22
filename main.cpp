#include "Camera.h"
#include "Mesh.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <glad/glad.h> 
#include <SDL2/SDL.h>

// Include the ImGui headers
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

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
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";

int main(int argc, char* argv[]) {
    // --- 1. System Initialization ---
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { return -1; }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

SDL_Window* window = SDL_CreateWindow("Blink3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);    if (!window) { return -1; }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) { return -1; }
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) { return -1; }

    // --- 2. Initialize Dear ImGui ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // --- Compile Shaders ---
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --- 3. Define the Mesh Data ---
    std::vector<Vertex> cubeVertices = {
        // Front face (Red)
        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},
        {glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},
        {glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},
        {glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},

        // Back face (Blue)
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f)},
        {glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f)},
        {glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f)},
        {glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f)},
        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f)}
    };

// --- Generate Grid Data ---
    std::vector<Vertex> gridVertices;
    float gridSize = 10.0f; // Total size
    int divisions = 20;     // Number of squares
    glm::vec3 gridColor = glm::vec3(0.4f, 0.4f, 0.4f); // Subtle gray

    for (int i = 0; i <= divisions; i++) {
        float pos = -gridSize + (i * (gridSize * 2.0f / divisions));
        
        // Lines along X axis
        gridVertices.push_back({glm::vec3(pos, 0.0f, -gridSize), gridColor});
        gridVertices.push_back({glm::vec3(pos, 0.0f, gridSize),  gridColor});
        
        // Lines along Z axis
        gridVertices.push_back({glm::vec3(-gridSize, 0.0f, pos), gridColor});
        gridVertices.push_back({glm::vec3(gridSize, 0.0f, pos),  gridColor});
    }

    // Create the grid mesh using GL_LINES
    Mesh grid(gridVertices, GL_LINES);

    Mesh myCube(cubeVertices);

    // --- 4. Main Application Loop Setup ---
// --- 4. Main Application Loop Setup ---
    glEnable(GL_DEPTH_TEST);
    
    // Camera focuses on center (0,0,0) from 4 units away
    Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), 4.0f);
    bool middleMouseDown = false;
    
    bool isRunning = true;
    SDL_Event event;

    while (isRunning) {

                static bool showGrid = true; // State for the toggle


        // Check if Shift is being held (for Panning)
        const Uint8* state = SDL_GetKeyboardState(NULL);
        bool isShift = state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT];

        // --- Handle Input Events ---
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) { isRunning = false; }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) { isRunning = false; }

            // Camera Controls (Only if ImGui isn't using the mouse)
            if (!io.WantCaptureMouse) {
                // Scroll Wheel to Zoom
                if (event.type == SDL_MOUSEWHEEL) {
                    camera.ProcessMouseScroll(event.wheel.y);
                }

                // Middle Mouse Click to Orbit/Pan
                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_MIDDLE) {
                    middleMouseDown = true;
                    SDL_SetRelativeMouseMode(SDL_TRUE); // Lock mouse to window
                }
                if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_MIDDLE) {
                    middleMouseDown = false;
                    SDL_SetRelativeMouseMode(SDL_FALSE); // Unlock mouse
                }

                // Mouse Movement
                if (event.type == SDL_MOUSEMOTION && middleMouseDown) {
                    if (isShift) {
                        // If Shift is held, Pan the camera
                        camera.ProcessMousePan(event.motion.xrel, -event.motion.yrel);
                    } else {
                        // Otherwise, Orbit the camera
                        camera.ProcessMouseOrbit(event.motion.xrel, -event.motion.yrel);
                    }
                }
            }
        }
        
        // --- ImGui Frame Setup ---
// --- ImGui Frame Setup ---
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // --- Build the Main Menu Bar ---
        if (ImGui::BeginMainMenuBar()) {
            
            // 1. The File Menu
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New", "Ctrl+N")) { /* TODO */ }
                if (ImGui::MenuItem("Open", "Ctrl+O")) { /* TODO */ }
                ImGui::Separator(); // Adds a nice horizontal line
                
                // Moved your export button here!
                if (ImGui::MenuItem("Export .OBJ")) { 
                    std::cout << "Exporting model..." << std::endl;
                }
                
                ImGui::Separator();
                
                // We can actually wire the Exit button up right now!
                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    isRunning = false; 
                }
                ImGui::EndMenu();
            }

            // 2. The Edit Menu
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) { /* TODO */ }
                // The 'false, false' makes the button grayed out (disabled)
                if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) { /* TODO */ } 
                ImGui::EndMenu();
            }

            // 3. The Window Menu
            if (ImGui::BeginMenu("Window")) {
                ImGui::MenuItem("Show Grid", nullptr, &showGrid); // Adds a checkbox
                if (ImGui::BeginMenu("Resolution")) {
                    if (ImGui::MenuItem("800 x 600")) {
                        SDL_SetWindowSize(window, 800, 600);
                        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                    }
                    if (ImGui::MenuItem("1024 x 768")) {
                        SDL_SetWindowSize(window, 1024, 768);
                        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                    }
                    if (ImGui::MenuItem("1280 x 720")) {
                        SDL_SetWindowSize(window, 1280, 720);
                        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                    }
                    if (ImGui::MenuItem("1920 x 1080")) {
                        SDL_SetWindowSize(window, 1920, 1080);
                        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                    }
                    ImGui::EndMenu();
                }

                ImGui::Separator();

                // Toggle Fullscreen Borderless
                static bool isFullscreen = false;
                if (ImGui::MenuItem("Toggle Fullscreen", "F11")) {
                    isFullscreen = !isFullscreen;
                    // SDL_WINDOW_FULLSCREEN_DESKTOP creates a borderless fullscreen window
                    SDL_SetWindowFullscreen(window, isFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Let's keep a tiny floating debug window just for your framerate
        ImGui::Begin("Debug Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize); 
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        ImGui::Render();
        // --- Render Frame ---
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        
        // MVP Matrices
        glm::mat4 model = glm::mat4(1.0f); 
        // Get View Matrix from our custom Camera!
        glm::mat4 view = camera.GetViewMatrix(); 
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


// Draw the Grid
    if (showGrid) {
        // We use the same shader! 
        // Identity matrix for model because the grid is already at origin
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        grid.Draw();
    }

    // Draw the Cube (after the grid)
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    myCube.Draw();

        // Draw UI
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // --- Cleanup ---
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}