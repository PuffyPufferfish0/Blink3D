#include "Toolbar.h"

Toolbar::Toolbar() : currentTool(ToolMode::MOVE) {}

void Toolbar::draw(int winH) {
    // Pin toolbar to the left side, slightly below the top menu bar
    ImGui::SetNextWindowPos(ImVec2(0, 24), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(75, winH - 24), ImGuiCond_Always);
    
    // Create a clean, borderless window panel
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
    
    // Helper lambda to draw highlighted tool buttons
    auto DrawToolButton = [&](const char* label, ToolMode mode) {
        if (currentTool == mode) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
        if (ImGui::Button(label, ImVec2(60, 60))) currentTool = mode;
        if (currentTool == mode) ImGui::PopStyleColor();
    };

    DrawToolButton("Move\n(T)", ToolMode::MOVE);
    ImGui::Dummy(ImVec2(0, 10)); 
    DrawToolButton("Rotate\n(R)", ToolMode::ROTATE);
    ImGui::Dummy(ImVec2(0, 10)); 
    DrawToolButton("Scale\n(E)", ToolMode::SCALE);
    ImGui::Dummy(ImVec2(0,10));
    DrawToolButton("split\n(q)", ToolMode::SPLIT);

    ImGui::End();
}