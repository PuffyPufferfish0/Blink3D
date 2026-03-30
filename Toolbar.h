#pragma once
#include "imgui/imgui.h"

enum class ToolMode { MOVE, ROTATE, SCALE };
enum class SelectMode { POINT, LINE }; // NEW!

class Toolbar {
public:
    ToolMode currentTool;
    SelectMode selectMode; // NEW!

    Toolbar();
    void draw(int winH);
};