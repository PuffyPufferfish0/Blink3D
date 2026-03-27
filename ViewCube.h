#pragma once
#include "Mesh.h"
#include "Camera.h"
#include <SDL2/SDL.h>

class ViewCube {
public:
    ViewCube();
    ~ViewCube();

    void init();
    void draw(Camera* cam, int winW, int winH, int drawW, int drawH, unsigned int shader);
    
    bool handleMousePress(int x, int y, int winW);
    bool handleMouseRelease();
    bool handleMouseMotion(int xrel, int yrel, Camera* cam);

private:
    Mesh* cubeMesh;
    bool isDragging;
    int size; // Size of the cube overlay in pixels
};