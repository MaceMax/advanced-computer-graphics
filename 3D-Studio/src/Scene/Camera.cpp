#include "Camera.hpp"

#include "3dStudio.hpp"

Camera::Camera(const glm::vec3& position, const glm::vec3& refPoint, float aspectRatio, float nearPlane, float farPlane) {
    this->position = position;
    this->cameraRef = refPoint;
    // Set the front vector to be the normalized vector from the camera position to the reference point.
    this->front = glm::normalize(this->cameraRef - this->position);
    this->worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    this->viewMatrix = glm::lookAt(this->position, cameraRef, worldUp);
    this->projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, nearPlane, farPlane);
}

Camera::~Camera() {
}

void Camera::Translate(glm::vec3 translation) {
    if (glm::compMax(translation) != 0 || glm::compMin(translation) != 0) {
        // translation should be relative to the camera's coordinate system
        glm::vec3 right = glm::normalize(glm::cross(this->front, this->worldUp));
        glm::vec3 up = glm::normalize(glm::cross(right, this->front));

        this->position += translation.x * right + translation.y * up + translation.z * this->front;
        this->cameraRef += translation.x * right + translation.y * up + translation.z * this->front;
        this->viewMatrix = glm::lookAt(this->position, this->cameraRef, this->worldUp);
    }
}

void Camera::Rotate(glm::vec3 frontVector) {
    if (frontVector != this->front) {
        this->cameraRef = this->position + this->front;
        this->front = glm::normalize(frontVector);
        this->viewMatrix = glm::lookAt(this->position, this->position + this->front, this->worldUp);
    }
}

void Camera::ProjectPerspective(float fov, float aspectRatio, float near, float far) {
    this->projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::ProjectOrthographic(float top, float near, float far, float obliqueAngle, float obliqueScale, float aspectRatio) {
    glm::mat4 shearMat = glm::mat4(1.0f);
    shearMat[2].x = obliqueScale * glm::cos(obliqueAngle);
    shearMat[2].y = obliqueScale * glm::sin(obliqueAngle);
    this->projectionMatrix = glm::ortho(-top * aspectRatio, top * aspectRatio, -top, top, near, far) * shearMat;
}

glm::mat4 Camera::GetViewMatrix() const {
    return this->viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return this->projectionMatrix;
}

glm::vec3 Camera::GetFront() const {
    return this->front;
}

glm::vec3 Camera::GetPosition() const {
    return this->position;
}

void Camera::Reset() {
    this->position = glm::vec3(0.0f, 0.0f, 2.0f);
    this->cameraRef = glm::vec3(0.0f, 0.0f, 0.0f);
    this->front = glm::normalize(this->cameraRef - this->position);
    this->worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    this->viewMatrix = glm::lookAt(this->position, this->cameraRef, this->worldUp);
}