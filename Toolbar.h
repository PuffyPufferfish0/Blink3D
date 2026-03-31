#pragma once
#include "imgui/imgui.h"

enum class ToolMode { MOVE, ROTATE, SCALE };
enum class SelectMode { POINT, LINE, FACE }; // Added Face Mode

class Toolbar {
public:
    ToolMode currentTool;
    SelectMode selectMode; 

    Toolbar();
    void draw(int winH);
};