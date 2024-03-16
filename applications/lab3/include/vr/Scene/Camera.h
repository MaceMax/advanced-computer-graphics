#pragma once

// clang-format off
#include <glm/glm.hpp>
#include <vr/State/Shader.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/gtc/constants.hpp>

namespace vr
{
  /**
  Class that defines a Camera for OpenGL.
  Handles keyboard events, handles Uniforms for view, projection and model matrix
  */
  class Camera
  {

  public:

    /// Constructor
    Camera();

    Camera(float fov, float near, float far, glm::vec3 position, glm::vec3 lookAt, bool movementEnabled = true);
    /**
    Handle keyboard events

    \param window The window for which keyboard events are handled
    */
    void processInput(GLFWwindow* window);

    /**
    Get the size of the window for this camera

    \return size of the window
    */
    glm::uvec2 getScreenSize();

    /**
    Set the size of the window
    \param size The new size
    */
    void setScreenSize(const glm::uvec2& size);

    /**
    Set the near/far distance of the projection volume
    \param nearFar New near far
    */
    void setNearFar(const glm::vec2& nearFar);

    /**
    Set the horizontal field of view
    \param fov The new fov
    */
    void setFOV(float fov);

    /**
    Get the horizontal field of view
    \return the current Field of View
    */
    float getFOV() const;

    /**
    Called when uniforms should be processed within the active program
    \param program
    */
    void apply(std::shared_ptr<vr::Shader> shader);

    /**
    Set the overall transform of the camera (position, up, direction)
    \param transform
    */
    void setTransform(glm::mat4 transform);

    /**
    Set the position, direction and up vector of the camera
    \param eye New position of the camera
    \param direction New direction
    \param up New up vector
    */
    void set(glm::vec3 eye, glm::vec3 direction, glm::vec3 up);

    /**
    Set the new position of the camera

    \param pos New position
    */
    void setPosition(glm::vec3 pos);

    /**
    Get the position of the camera

    \return Current position of the camera
    */
    glm::vec3 getPosition() const;

    /**
    Set the mouse speed when changing direction.
    Higher value means faster rotation. Default is 1.

    \param speed - The speed of mouse movements
    */
    void setMouseSpeed(float speed);

    /**
    Get the mouse speed when changing direction.
    \return The speed of mouse movements
    */
    float getMouseSpeed() const;

    /**
    Set the translational speed
    Higher value means faster speed.

    \param speed - The speed of translational movements.
    */
    void setSpeed(float speed);

    /**
    Get the translational speed.
    \return The speed of translational movements
    */
    float getSpeed() const;

    /**
     * @brief Get the View matrix
     * 
     * @return glm::mat4  The view matrix
     */ 
    glm::mat4 getView() const;

    /**
     * @brief Get the Projection matrix
     * 
     * @return glm::mat4  The projection matrix
     */
    glm::mat4 getProjection() const;

    /**
     * @brief Set the Aperture of the camera
     * 
     * @param aperture  The aperture
     */
    void setAperture(float aperture);


  private:

    void handleMouse(GLFWwindow* window, double xpos, double ypos);

    GLuint m_fbo;
    GLuint m_rbo;
    GLuint m_depthTexure;

    glm::mat4 m_view;
    glm::mat4 m_projection;
    glm::mat4 m_transform;

    glm::uvec2 m_screenSize;
    glm::vec2 m_nearFar;

    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_up;

    // Depth of field
    float aperture;

    bool m_movementEnabled;
    bool m_firstClick;
    float m_speed;
    float m_fov;
    float m_horizontalAngle;
    float m_verticalAngle;
    float m_mouseSpeed;
    float m_lastTime;
    float m_speedup;
    double m_lastX;
    double m_lastY;
  };

    typedef std::vector<std::shared_ptr<Camera>> CameraVector;
}