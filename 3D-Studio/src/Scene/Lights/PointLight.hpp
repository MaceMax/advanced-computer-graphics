#include "Light.hpp"

/**
 * A point light. This light has a position and attenuation.
 */
class PointLight : public Light {
   public:
    PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position);

    glm::vec3 GetPosition() const;
    void SetPosition(float x, float y, float z);
    float GetConstant();
    void SetConstant(float constant);
    float GetLinear();
    void SetLinear(float linear);
    float GetQuadratic();
    void SetQuadratic(float quadratic);

   private:
    glm::vec3 position;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};
