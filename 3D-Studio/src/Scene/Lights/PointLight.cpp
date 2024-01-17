#include "PointLight.hpp"

PointLight::PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position) {
    this->position = position;
    this->type = LightType::POINT;
    this->ambientIntensity = ambient;
    this->diffuseIntensity = diffuse;
    this->specularIntensity = specular;
}

glm::vec3 PointLight::GetPosition() const {
    return position;
}

void PointLight::SetPosition(float x, float y, float z) {
    this->position = glm::vec3(x, y, z);
}

float PointLight::GetConstant() {
    return constant;
}

void PointLight::SetConstant(float constant) {
    this->constant = constant;
}

float PointLight::GetLinear() {
    return linear;
}

void PointLight::SetLinear(float linear) {
    this->linear = linear;
}

float PointLight::GetQuadratic() {
    return quadratic;
}

void PointLight::SetQuadratic(float quadratic) {
    this->quadratic = quadratic;
}