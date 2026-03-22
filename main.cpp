#include <iostream>
// Include GLAD first, then SDL
#include <glad/glad.h> 
#include <SDL2/SDL.h>

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

int main(int argc, char* argv[]) {
    // 1. Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL failed to initialize: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 2. Set OpenGL Attributes
    // We are requesting OpenGL 3.3 Core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    // Enable double buffering to prevent screen tearing
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // 3. Create the Window
    SDL_Window* window = SDL_CreateWindow(
        "N64 Modeler", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        WINDOW_WIDTH, 
        WINDOW_HEIGHT, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN // Crucial: flag it for OpenGL
    );

    if (!window) {
        std::cerr << "Window failed to create: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // 4. Create the OpenGL Context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context failed to create: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 5. Load OpenGL Functions using GLAD
    // We pass SDL's function loader to GLAD so it knows how to map the pointers
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    std::cout << "Successfully loaded OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // 6. The Main Application Loop
    bool isRunning = true;
    SDL_Event event;

    while (isRunning) {
        // Handle Inputs and Window Events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            // If the user presses Escape, close the app
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                isRunning = false;
            }
        }

        // --- Rendering Code Goes Here ---
        
        // Set the clear color (let's use a classic N64 UI dark blue)
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Swap the front and back buffers to display what we just rendered
        SDL_GL_SwapWindow(window);
    }

    // 7. Cleanup
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}