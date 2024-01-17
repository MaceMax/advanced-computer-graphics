#include "SpotLight.hpp"

SpotLight::SpotLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, glm::vec3 direction,
                     float cutoff, float outerCutoff) {
    this->position = position;
    this->direction = direction;
    this->cutoff = cutoff;
    this->outerCutoff = outerCutoff;
    this->type = LightType::SPOT;
    this->ambientIntensity = ambient;
    this->diffuseIntensity = diffuse;
    this->specularIntensity = specular;
}

glm::vec3 SpotLight::GetPosition() const {
    return position;
}

void SpotLight::SetPosition(float x, float y, float z) {
    this->position = glm::vec3(x, y, z);
}

glm::vec3 SpotLight::GetDirection() const {
    return direction;
}

void SpotLight::SetDirection(float x, float y, float z) {
    this->direction = glm::vec3(x, y, z);
}

float SpotLight::GetCutoff() {
    return cutoff;
}

void SpotLight::SetCutoff(float cutoff) {
    this->cutoff = cutoff;
}

float SpotLight::GetOuterCutoff() {
    return outerCutoff;
}

void SpotLight::SetOuterCutoff(float outerCutoff) {
    this->outerCutoff = outerCutoff;
}
