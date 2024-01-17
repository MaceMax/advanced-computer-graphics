#include "Light.hpp"

#include "3dStudio.hpp"

Light::Light() {}

glm::vec3 Light::GetAmbientIntensity() const {
    return ambientIntensity;
}

void Light::SetAmbientIntensity(float r, float g, float b) {
    this->ambientIntensity = glm::vec3(r, g, b);
}

glm::vec3 Light::GetDiffuseIntensity() const {
    return diffuseIntensity;
}

void Light::SetDiffuseIntensity(float r, float g, float b) {
    this->diffuseIntensity = glm::vec3(r, g, b);
}

glm::vec3 Light::GetSpecularIntensity() const {
    return specularIntensity;
}

void Light::SetSpecularIntensity(float r, float g, float b) {
    this->specularIntensity = glm::vec3(r, g, b);
}

LightType Light::GetType() const {
    return type;
}
