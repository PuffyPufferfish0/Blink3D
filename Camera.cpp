#include "Camera.h"

Camera::Camera(glm::vec3 target, float distance) {
    Target = target;
    Distance = distance;
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Yaw = -90.0f; 
    Pitch = 0.0f;
    
    MouseSensitivity = 0.2f; // Slightly faster rotation
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
    // Constrain pitch to avoid flipping
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

    // Constrain pitch so we don't flip upside down
    if (Pitch > 89.0f)  Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    updateCameraVectors();
}

void Camera::ProcessMousePan(float xoffset, float yoffset) {
    // Move the actual target point along our Right and Up vectors
    Target -= Right * (xoffset * PanSpeed);
    Target -= Up * (yoffset * PanSpeed); 
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    // Zoom in and out
    Distance -= yoffset * ZoomSpeed;
    if (Distance < 0.5f) Distance = 0.5f; // Prevent zooming through the model
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    
    // Re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp)); 
    Up    = glm::normalize(glm::cross(Right, Front));

    // Lock the camera's position to the orbit sphere around the Target
    Position = Target - (Front * Distance);
}