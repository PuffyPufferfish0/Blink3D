#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Point.h"
#include "Line.h"
#include <vector>
#include <glm/glm.hpp>

class TransformGizmo {
public:
    enum Axis { NONE, X, Y, Z };
    Axis activeAxis;
    glm::vec3 position;
    bool isDragging;
    bool isVisible;

    TransformGizmo();
    ~TransformGizmo();

    void init();
    void updateState(const std::vector<Point>& points, const std::vector<Line>& lines);
    void draw(Camera* cam, int winW, int winH, unsigned int shader);

    bool handleMousePress(int mouseX, int mouseY, Camera* cam, int winW, int winH);
    bool handleMouseRelease();
    bool handleMouseMotion(int xrel, int yrel, Camera* cam, int winW, int winH, std::vector<Point>& points, const std::vector<Line>& lines);

private:
    Mesh* gizmoMeshX;
    Mesh* gizmoMeshY;
    Mesh* gizmoMeshZ;
    
    float distToSegment(glm::vec2 p, glm::vec2 v, glm::vec2 w);
    std::vector<Vertex> buildAxisLine(glm::vec3 dir, glm::vec3 color);
};