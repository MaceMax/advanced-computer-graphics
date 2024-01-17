#pragma once
#include "3dStudio.hpp"

/**
 * A base class for all lights. Every light has an intensity and a type.
 */
class Light {
   public:
    Light();
    virtual ~Light() = default;

    glm::vec3 GetIntensity() const;
    void SetIntensity(float r, float g, float b);
    glm::vec3 GetAmbientIntensity() const;
    void SetAmbientIntensity(float r, float g, float b);
    glm::vec3 GetDiffuseIntensity() const;
    void SetDiffuseIntensity(float r, float g, float b);
    glm::vec3 GetSpecularIntensity() const;
    void SetSpecularIntensity(float r, float g, float b);
    LightType GetType() const;

   protected:
    glm::vec3 ambientIntensity;
    glm::vec3 diffuseIntensity;
    glm::vec3 specularIntensity;
    LightType type;
};