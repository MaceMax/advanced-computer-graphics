#pragma once
#include "3dStudio.hpp"

/**
 * Camera class. This class is a representation of a camera in the scene.
 * It stores the position of the camera, the reference point, the up vector,
 * the front vector, the view matrix and the projection matrix.
 */
class Camera {
   public:
    /**
     * @brief Construct a new Camera. The default projection is perspective.
     *
     * @param position The position of the camera
     * @param refPoint The reference point of the camera
     * @param aspectRatio The aspect ratio of the camera
     * @param nearPlane The near plane of the camera
     * @param farPlane The far plane of the camera
     */
    Camera(const glm::vec3& position, const glm::vec3& refPoint, float aspectRatio, float nearPlane, float farPlane);
    ~Camera();

    /**
     * @brief Translates the camera
     *
     * @param translation The translation vector
     */
    void Translate(glm::vec3 translation);

    /**
     * @brief Rotates the camera
     *
     * @param frontVector The new front vector
     */
    void Rotate(glm::vec3 frontVector);

    /**
     * @brief Projects the camera in perspective mode
     *
     * @param fov The field of view
     * @param aspectRatio The aspect ratio
     * @param near The near plane
     * @param far The far plane
     */
    void ProjectPerspective(float fov, float aspectRatio, float near, float far);

    /**
     * @brief Projects the camera in orthographic mode
     *
     * @param top The top plane
     * @param near The near plane
     * @param far The far plane
     * @param obliqueAngle The oblique angle
     * @param obliqueScale The oblique scale
     * @param aspectRatio The aspect ratio
     */
    void ProjectOrthographic(float top, float near, float far, float obliqueAngle, float obliqueScale, float aspectRatio);

    /**
     * @brief Get the view matrix
     *
     * @return glm::mat4 The view matrix
     */
    glm::mat4 GetViewMatrix() const;

    /**
     * @brief Get the projection matrix
     *
     * @return glm::mat4 The projection matrix
     */
    glm::mat4 GetProjectionMatrix() const;

    /**
     * @brief Get the front vector
     *
     * @return glm::vec3 The front vector
     */
    glm::vec3 GetFront() const;

    /**
     * @brief Get the position of the camera
     *
     * @return glm::vec3 The position of the camera
     */
    glm::vec3 GetPosition() const;
    void Reset();

   private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 cameraRef;
    glm::vec3 up;
    glm::vec3 worldUp;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};