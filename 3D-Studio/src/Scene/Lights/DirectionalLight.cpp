#include "DirectionalLight.hpp"

DirectionalLight::DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction) {
    this->ambientIntensity = ambient;
    this->diffuseIntensity = diffuse;
    this->specularIntensity = specular;
    this->direction = direction;
    this->type = LightType::DIRECTIONAL;
}

glm::vec3 DirectionalLight::GetDirection() const {
    return direction;
}

void DirectionalLight::SetDirection(float x, float y, float z) {
    this->direction = glm::vec3(x, y, z);
}