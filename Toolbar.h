#pragma once
#include "imgui/imgui.h"

enum class ToolMode { 
    MOVE, 
    ROTATE, 
    SCALE,
    SPLIT
};

class Toolbar {
public:
    ToolMode currentTool;

    Toolbar();
    void draw(int winH);
};