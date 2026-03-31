#include "AddMenu.h"
#include "imgui/imgui.h"

AddMenu::AddMenu() : isOpen(false) {}

bool AddMenu::draw(std::vector<Point>& points, std::vector<unsigned int>& indices) {
    if (!isOpen) return false;

    bool sceneChanged = false;

    // Center the menu exactly in the middle of the screen when it pops up
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    
    ImGui::Begin("Add Object", &isOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
    
    ImGui::Text("Primitives");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 5));

    if (ImGui::Button("Triangle (2D)", ImVec2(150, 40))) {
        unsigned int startIdx = points.size();

        // 1. Inject the 3 vertices of a simple equilateral-style triangle at the origin
        points.push_back(Point(glm::vec3(0.0f, 0.5f, 0.0f)));
        points.push_back(Point(glm::vec3(-0.5f, -0.5f, 0.0f)));
        points.push_back(Point(glm::vec3(0.5f, -0.5f, 0.0f)));

        // 2. Inject the counter-clockwise winding order so OpenGL renders the face
        indices.push_back(startIdx);
        indices.push_back(startIdx + 1);
        indices.push_back(startIdx + 2);

        sceneChanged = true;
        isOpen = false; // Auto-close the menu after spawning
    }
    
    ImGui::Dummy(ImVec2(0, 5));
    ImGui::TextDisabled("More shapes coming soon...");

    ImGui::End();
    
    return sceneChanged;
}