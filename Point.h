#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>

class Point {
public:
    glm::vec3 position;
    glm::vec3 color;
    bool selected;

    // Default colors
    static const glm::vec3 COLOR_DEFAULT;
    static const glm::vec3 COLOR_SELECTED;

    Point(glm::vec3 pos);
    
    void select();
    void deselect();
    void toggleSelection();
    
    // Updates color based on selection state
    void updateState();
};