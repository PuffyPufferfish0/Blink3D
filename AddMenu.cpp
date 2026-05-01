#include "AddMenu.h"
#include "imgui/imgui.h"

AddMenu::AddMenu() : isOpen(false) {}

bool AddMenu::draw(std::vector<Point>& points, std::vector<unsigned int>& indices) {
    if (!isOpen) return false;

    bool sceneChanged = false;

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    
    ImGui::Begin("Add Object", &isOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
    
    ImGui::Text("Primitives");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 5));

    if (ImGui::Button("Triangle (2D)", ImVec2(150, 40))) {
        unsigned int startIdx = points.size();

        points.push_back(Point(glm::vec3(0.0f, 0.5f, 0.0f)));
        points.push_back(Point(glm::vec3(-0.5f, -0.5f, 0.0f)));
        points.push_back(Point(glm::vec3(0.5f, -0.5f, 0.0f)));

        indices.push_back(startIdx);
        indices.push_back(startIdx + 1);
        indices.push_back(startIdx + 2);

        sceneChanged = true;
        isOpen = false;
    }
    
    ImGui::Dummy(ImVec2(0, 5));
    ImGui::TextDisabled("More shapes coming soon...");

    ImGui::End();
    
    return sceneChanged;
}