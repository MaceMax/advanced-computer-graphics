#include <vr/Scene/Light.h>
#include <vr/glErrorUtil.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

using namespace vr;

Light::Light(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular) : enabled(true) {
    this->position = position;
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;

    this->constant = 1.0f;
    this->linear = 0.09f;
    this->quadratic = 0.032f;
}

void Light::setEnabled(bool enabled) {
    this->enabled = enabled;
}

void Light::setPosition(glm::vec4 position) {
    this->position = position;
}

void Light::setAmbient(glm::vec4 ambient) {
    this->ambient = ambient;
}

void Light::setDiffuse(glm::vec4 diffuse) {
    this->diffuse = diffuse;
}

void Light::setSpecular(glm::vec4 specular) {
    this->specular = specular;
}

void Light::setAttenuation(float constant, float linear, float quadratic) {
    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;
}

void Light::apply(std::shared_ptr<vr::Shader> shader, size_t idx) {
    std::stringstream str;
    str << "lights[" << idx << "].";
    std::string prefix = str.str();

    GLint loc = -1;
    std::string uniform_name;

    uniform_name = prefix + "enabled";
    shader->setInt(uniform_name, enabled);
    shader->setVec4(prefix + "ambient", this->ambient);
    shader->setVec4(prefix + "diffuse", this->diffuse);
    shader->setVec4(prefix + "specular", this->specular);
    shader->setVec4(prefix + "position", this->position);

    if (position.w != 0) {
        shader->setFloat(prefix + "constant", this->constant);
        shader->setFloat(prefix + "linear", this->linear);
        shader->setFloat(prefix + "quadratic", this->quadratic);
    }
    CHECK_GL_ERROR_LINE_FILE();
}
