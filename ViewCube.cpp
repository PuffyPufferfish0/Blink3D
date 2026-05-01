#include "ViewCube.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ViewCube::ViewCube() : cubeMesh(nullptr), isDragging(false), size(120) {}

ViewCube::~ViewCube() {
    if (cubeMesh) delete cubeMesh;
}

void ViewCube::init() {
    std::vector<Vertex> verts;
    glm::vec3 pts[8] = {
        {-0.5, -0.5,  0.5}, { 0.5, -0.5,  0.5}, { 0.5,  0.5,  0.5}, {-0.5,  0.5,  0.5},
        {-0.5, -0.5, -0.5}, { 0.5, -0.5, -0.5}, { 0.5,  0.5, -0.5}, {-0.5,  0.5, -0.5}
    };
    for(int i=0; i<8; i++) verts.push_back({pts[i], glm::vec3(0.6f)}); 

    std::vector<unsigned int> indices = {
        0,1,2, 2,3,0, 1,5,6, 6,2,1, 7,6,5, 5,4,7, 4,0,3, 3,7,4, 3,2,6, 6,7,3, 4,5,1, 1,0,4
    };
    cubeMesh = new Mesh(verts, GL_TRIANGLES, indices);
}

bool ViewCube::handleMousePress(int x, int y, int winW) {
    if (x > winW - size && y < size) {
        isDragging = true;
        return true; 
    }
    return false;
}

bool ViewCube::handleMouseRelease() {
    if (isDragging) {
        isDragging = false;
        return true;
    }
    return false;
}

bool ViewCube::handleMouseMotion(int xrel, int yrel, Camera* cam) {
    if (isDragging) {
        cam->ProcessMouseOrbit(xrel, -yrel); 
        return true;
    }
    return false;
}

void ViewCube::draw(Camera* cam, int winW, int winH, int drawW, int drawH, GLuint shader) {
    float scaleX = (float)drawW / (float)winW;
    float scaleY = (float)drawH / (float)winH;

    int vpSizeX = size * scaleX;
    int vpSizeY = size * scaleY;
    
    glViewport(drawW - vpSizeX, drawH - vpSizeY, vpSizeX, vpSizeY);
    
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 proj = glm::ortho(-0.8f, 0.8f, -0.8f, 0.8f, 0.1f, 10.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0,0,0) - cam->Front * 3.0f, glm::vec3(0,0,0), cam->Up);
    glm::mat4 model = glm::mat4(1.0f);

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glUniform1i(glGetUniformLocation(shader, "isPoint"), false);
    glUniform1i(glGetUniformLocation(shader, "overrideColor"), true);
    glUniform3f(glGetUniformLocation(shader, "colorVec"), 0.5f, 0.5f, 0.5f);
    cubeMesh->drawMode = GL_TRIANGLES;
    cubeMesh->Draw();

    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0f, -1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUniform3f(glGetUniformLocation(shader, "colorVec"), 0.1f, 0.1f, 0.1f);
    cubeMesh->Draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);

    glm::vec3 faceCenters[6] = {
        {0, 0, 0.5f}, {0, 0, -0.5f}, {0, 0.5f, 0}, {0, -0.5f, 0}, {0.5f, 0, 0}, {-0.5f, 0, 0}
    };
    const char* faceNames[6] = {"FRONT", "BACK", "TOP", "BOTTOM", "RIGHT", "LEFT"};

    for (int i=0; i<6; i++) {
        glm::vec3 normal = glm::normalize(faceCenters[i]);
        if (glm::dot(normal, -cam->Front) > 0.5f) { 
            glm::vec4 clip = proj * view * glm::vec4(faceCenters[i], 1.0f);
            glm::vec3 ndc = glm::vec3(clip) / clip.w;
            
            float screenX = (winW - size) + (ndc.x + 1.0f) * 0.5f * size;
            float screenY = 0 + (1.0f - ndc.y) * 0.5f * size; 

            ImVec2 textSize = ImGui::CalcTextSize(faceNames[i]);
            ImGui::GetForegroundDrawList()->AddText(
                ImVec2(screenX - textSize.x/2.0f, screenY - textSize.y/2.0f), 
                IM_COL32(255, 255, 255, 255), 
                faceNames[i]
            );
        }
    }
}