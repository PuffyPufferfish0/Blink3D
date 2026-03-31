#include "TransformGizmo.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

TransformGizmo::TransformGizmo() : activeAxis(NONE), isDragging(false), isVisible(false), 
                                   gizmoMeshX(nullptr), gizmoMeshY(nullptr), gizmoMeshZ(nullptr) {}

TransformGizmo::~TransformGizmo() {
    if (gizmoMeshX) delete gizmoMeshX;
    if (gizmoMeshY) delete gizmoMeshY;
    if (gizmoMeshZ) delete gizmoMeshZ;
}

std::vector<Vertex> TransformGizmo::buildAxisLine(glm::vec3 dir, glm::vec3 color) {
    std::vector<Vertex> v;
    v.push_back({{0,0,0}, color});
    v.push_back({dir * 0.8f, color});
    
    glm::vec3 up = glm::vec3(0,1,0);
    if (glm::abs(glm::dot(dir, up)) > 0.99f) up = glm::vec3(1,0,0);
    
    glm::vec3 right = glm::normalize(glm::cross(dir, up)) * 0.08f;
    glm::vec3 forward = glm::normalize(glm::cross(right, dir)) * 0.08f;

    v.push_back({dir * 0.8f + right, color}); v.push_back({dir, color});
    v.push_back({dir * 0.8f - right, color}); v.push_back({dir, color});
    v.push_back({dir * 0.8f + forward, color}); v.push_back({dir, color});
    v.push_back({dir * 0.8f - forward, color}); v.push_back({dir, color});
    return v;
}

void TransformGizmo::init() {
    gizmoMeshX = new Mesh(buildAxisLine({1,0,0}, {1,0,0}), GL_LINES);
    gizmoMeshY = new Mesh(buildAxisLine({0,1,0}, {0,1,0}), GL_LINES);
    gizmoMeshZ = new Mesh(buildAxisLine({0,0,1}, {0,0,1}), GL_LINES);
}

void TransformGizmo::updateState(const std::vector<Point>& points, const std::vector<Line>& lines, const std::vector<Face>& faces) {
    glm::vec3 avg(0.0f);
    int count = 0;
    
    std::vector<bool> moveFlag(points.size(), false);
    for (size_t i = 0; i < points.size(); i++) if (points[i].selected) moveFlag[i] = true;
    for (const auto& l : lines) if (l.selected) { moveFlag[l.v1] = true; moveFlag[l.v2] = true; }
    for (const auto& f : faces) if (f.selected) { moveFlag[f.v1] = true; moveFlag[f.v2] = true; moveFlag[f.v3] = true; }

    for (size_t i = 0; i < points.size(); i++) {
        if (moveFlag[i]) { avg += points[i].position; count++; }
    }
    
    isVisible = (count > 0);
    if (isVisible) position = avg / (float)count; 
}

void TransformGizmo::draw(Camera* cam, int winW, int winH, unsigned int shader) {
    if (!isVisible) return;
    
    glClear(GL_DEPTH_BUFFER_BIT); 

    float scale = cam->Distance * 0.15f; 
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(scale));

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shader, "isPoint"), false);
    glUniform1i(glGetUniformLocation(shader, "overrideColor"), true);

    glUniform3f(glGetUniformLocation(shader, "colorVec"), activeAxis == X ? 1.0f : 0.9f, activeAxis == X ? 1.0f : 0.1f, 0.1f);
    gizmoMeshX->Draw();
    
    glUniform3f(glGetUniformLocation(shader, "colorVec"), activeAxis == Y ? 1.0f : 0.1f, activeAxis == Y ? 1.0f : 0.9f, 0.1f);
    gizmoMeshY->Draw();
    
    glUniform3f(glGetUniformLocation(shader, "colorVec"), activeAxis == Z ? 1.0f : 0.1f, activeAxis == Z ? 1.0f : 0.1f, 0.9f);
    gizmoMeshZ->Draw();
}

float TransformGizmo::distToSegment(glm::vec2 p, glm::vec2 v, glm::vec2 w) {
    float l2 = glm::distance(v, w) * glm::distance(v, w);
    if (l2 == 0.0f) return glm::distance(p, v);
    float t = std::max(0.0f, std::min(1.0f, glm::dot(p - v, w - v) / l2));
    glm::vec2 projection = v + t * (w - v);
    return glm::distance(p, projection);
}

bool TransformGizmo::handleMousePress(int mouseX, int mouseY, Camera* cam, int winW, int winH) {
    if (!isVisible) return false;

    glm::mat4 v = cam->GetViewMatrix();
    glm::mat4 p = glm::perspective(glm::radians(45.0f), (float)winW / winH, 0.1f, 100.0f);
    float scale = cam->Distance * 0.15f;
    
    auto project = [&](glm::vec3 pos) -> glm::vec2 {
        glm::vec4 clip = p * v * glm::vec4(pos, 1.0f);
        if (clip.w == 0.0f) return glm::vec2(0);
        glm::vec3 ndc = glm::vec3(clip) / clip.w;
        return glm::vec2((ndc.x + 1.0f) * 0.5f * winW, (1.0f - ndc.y) * 0.5f * winH);
    };

    glm::vec2 o2D = project(position);
    glm::vec2 x2D = project(position + glm::vec3(1, 0, 0) * scale);
    glm::vec2 y2D = project(position + glm::vec3(0, 1, 0) * scale);
    glm::vec2 z2D = project(position + glm::vec3(0, 0, 1) * scale);

    glm::vec2 m(mouseX, mouseY);
    float dX = distToSegment(m, o2D, x2D);
    float dY = distToSegment(m, o2D, y2D);
    float dZ = distToSegment(m, o2D, z2D);

    float minDist = 20.0f; 
    activeAxis = NONE;

    if (dX < minDist) { activeAxis = X; minDist = dX; }
    if (dY < minDist) { activeAxis = Y; minDist = dY; }
    if (dZ < minDist) { activeAxis = Z; minDist = dZ; }

    if (activeAxis != NONE) {
        isDragging = true;
        return true; 
    }
    return false;
}

bool TransformGizmo::handleMouseRelease() {
    if (isDragging) {
        isDragging = false;
        activeAxis = NONE;
        return true;
    }
    return false;
}

bool TransformGizmo::handleMouseMotion(int xrel, int yrel, Camera* cam, int winW, int winH, std::vector<Point>& points, const std::vector<Line>& lines, const std::vector<Face>& faces) {
    if (!isDragging || activeAxis == NONE) return false;

    glm::mat4 v = cam->GetViewMatrix();
    glm::mat4 p = glm::perspective(glm::radians(45.0f), (float)winW / winH, 0.1f, 100.0f);
    float scale = cam->Distance * 0.15f;
    
    glm::vec3 dir(0.0f);
    if (activeAxis == X) dir = glm::vec3(1, 0, 0);
    if (activeAxis == Y) dir = glm::vec3(0, 1, 0);
    if (activeAxis == Z) dir = glm::vec3(0, 0, 1);

    auto project = [&](glm::vec3 pos) -> glm::vec2 {
        glm::vec4 clip = p * v * glm::vec4(pos, 1.0f);
        if (clip.w == 0.0f) return glm::vec2(0);
        glm::vec3 ndc = glm::vec3(clip) / clip.w;
        return glm::vec2((ndc.x + 1.0f) * 0.5f * winW, (1.0f - ndc.y) * 0.5f * winH);
    };

    glm::vec2 p0 = project(position);
    glm::vec2 p1 = project(position + dir * scale);
    glm::vec2 axis2D = p1 - p0;

    float lenSq = glm::dot(axis2D, axis2D);
    if (lenSq > 0.0001f) {
        glm::vec2 mouseDelta(xrel, yrel);
        float moveAmount = glm::dot(mouseDelta, axis2D) / lenSq;
        float trueAmount = moveAmount * scale;
        
        std::vector<bool> moveFlag(points.size(), false);
        for (size_t i = 0; i < points.size(); i++) if (points[i].selected) moveFlag[i] = true;
        for (const auto& l : lines) if (l.selected) { moveFlag[l.v1] = true; moveFlag[l.v2] = true; }
        for (const auto& f : faces) if (f.selected) { moveFlag[f.v1] = true; moveFlag[f.v2] = true; moveFlag[f.v3] = true; }
        
        for (size_t i = 0; i < points.size(); i++) {
            if (moveFlag[i]) points[i].position += dir * trueAmount;
        }
        position += dir * trueAmount; 
    }
    return true; 
}