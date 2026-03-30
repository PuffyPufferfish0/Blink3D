#pragma once
#include "imgui/imgui.h"

enum class ToolMode { 
    MOVE, 
    ROTATE, 
    SCALE 
};

class Toolbar {
public:
    ToolMode currentTool;

    Toolbar();
    void draw(int winH);
};