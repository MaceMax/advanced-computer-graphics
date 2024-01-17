#include "Light.hpp"

/**
 * A spot light. This light has a position, direction, cutoff and a outer cutoff.
 */

class SpotLight : public Light {
   public:
    SpotLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, glm::vec3 direction,
              float cutoff, float outerCutoff);

    /**
     * @brief Get the spotlight position
     *
     * @return glm::vec3 position of the spotlight
     */
    glm::vec3 GetPosition() const;

    /**
     * @brief Set the spotlight position
     *
     * @param x x position
     * @param y y position
     * @param z z position
     */
    void SetPosition(float x, float y, float z);

    /**
     * @brief Get the spotlight direction
     *
     * @return glm::vec3 direction of the spotlight
     */
    glm::vec3 GetDirection() const;

    /**
     * @brief Set the spotlight direction
     *
     * @param x x direction
     * @param y y direction
     * @param z z direction
     */
    void SetDirection(float x, float y, float z);

    /**
     * @brief Get the spotlight cutoff
     *
     * @return float cutoff of the spotlight
     */
    float GetCutoff();

    /**
     * @brief Set the spotlight cutoff
     *
     * @param cutoff cutoff of the spotlight
     */
    void SetCutoff(float cutoff);

    /**
     * @brief Get the spotlight outer cutoff
     *
     * @return float outer cutoff of the spotlight
     */
    float GetOuterCutoff();

    /**
     * @brief Set the spotlight outer cutoff
     *
     * @param outerCutoff outer cutoff of the spotlight
     */
    void SetOuterCutoff(float outerCutoff);

   private:
    glm::vec3 position;
    glm::vec3 direction;

    float cutoff;
    float outerCutoff;
};