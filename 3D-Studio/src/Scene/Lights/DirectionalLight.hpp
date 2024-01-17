#include "Light.hpp"

/**
 * A directional light. This light has a direction.
 */
class DirectionalLight : public Light {
   public:
    DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction);

    glm::vec3 GetDirection() const;
    void SetDirection(float x, float y, float z);

   private:
    glm::vec3 direction;
};