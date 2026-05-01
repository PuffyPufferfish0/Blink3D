#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

class Camera
{
public:
    void Reset();
    void SetRotation(float yaw, float pitch);

    glm::vec3 Position;
    glm::vec3 Target; 
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float Distance; 

    float MouseSensitivity;
    float ZoomSpeed;
    float PanSpeed;

    Camera(glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f), float distance = 4.0f);

    glm::mat4 GetViewMatrix();

    void ProcessMouseOrbit(float xoffset, float yoffset);
    void ProcessMousePan(float xoffset, float yoffset);
    void ProcessMouseScroll(float yoffset);

private:
    void updateCameraVectors();
};