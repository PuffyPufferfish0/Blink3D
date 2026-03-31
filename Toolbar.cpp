#include "Toolbar.h"

Toolbar::Toolbar() : currentTool(ToolMode::MOVE), selectMode(SelectMode::POINT) {}

void Toolbar::draw(int winH) {
    ImGui::SetNextWindowPos(ImVec2(0, 24), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(80, winH - 24), ImGuiCond_Always);
    
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
    
    auto DrawToolButton = [&](const char* label, ToolMode mode) {
        if (currentTool == mode) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
        if (ImGui::Button(label, ImVec2(64, 64))) currentTool = mode;
        if (currentTool == mode) ImGui::PopStyleColor();
    };

    auto DrawSelectButton = [&](const char* label, SelectMode mode) {
        if (selectMode == mode) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
        if (ImGui::Button(label, ImVec2(64, 40))) selectMode = mode;
        if (selectMode == mode) ImGui::PopStyleColor();
    };

    ImGui::Text("SELECT");
    DrawSelectButton("Pts (1)", SelectMode::POINT);
    ImGui::Dummy(ImVec2(0, 2));
    DrawSelectButton("Lns (2)", SelectMode::LINE);
    ImGui::Dummy(ImVec2(0, 2));
    DrawSelectButton("Fcs (3)", SelectMode::FACE); // Added Face button
    
    ImGui::Dummy(ImVec2(0, 10));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 10));

    ImGui::Text("TOOLS");
    DrawToolButton("Move\n(T)", ToolMode::MOVE);
    ImGui::Dummy(ImVec2(0, 5)); 
    DrawToolButton("Rotate\n(R)", ToolMode::ROTATE);
    ImGui::Dummy(ImVec2(0, 5)); 
    DrawToolButton("Scale\n(E)", ToolMode::SCALE);
    ImGui::Dummy(ImVec2(0, 5)); 
    
    if (ImGui::Button("Split\n(Q)", ImVec2(64, 64))) {}

    ImGui::End();
}