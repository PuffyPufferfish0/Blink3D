#include "Camera.h"

Camera::Camera(glm::vec3 target, float distance) {
    Target = target;
    Distance = distance;
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Yaw = -90.0f; 
    Pitch = 0.0f;
    
    MouseSensitivity = 0.2f; 
    ZoomSpeed = 0.5f;
    PanSpeed = 0.01f;
    
    updateCameraVectors();
}
void Camera::Reset() {
    Target = glm::vec3(0.0f, 0.0f, 0.0f);
    Distance = 4.0f;
    Yaw = -90.0f;
    Pitch = 0.0f;
    updateCameraVectors();
}

void Camera::SetRotation(float yaw, float pitch) {
    Yaw = yaw;
    Pitch = pitch;
    if (Pitch > 89.0f)  Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Target, Up);
}

void Camera::ProcessMouseOrbit(float xoffset, float yoffset) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (Pitch > 89.0f)  Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    updateCameraVectors();
}

void Camera::ProcessMousePan(float xoffset, float yoffset) {
    Target -= Right * (xoffset * PanSpeed);
    Target -= Up * (yoffset * PanSpeed); 
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    Distance -= yoffset * ZoomSpeed;
    if (Distance < 0.5f) Distance = 0.5f; 
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    
    Right = glm::normalize(glm::cross(Front, WorldUp)); 
    Up    = glm::normalize(glm::cross(Right, Front));

    Position = Target - (Front * Distance);
}