#pragma once

// Important: Import glad as first gl header
#include <glad/glad.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Shader.h"
#include "Texture.h"
namespace vr {

/// Simple class for storing material properties
class Material {
   private:
    glm::vec4 m_ambient, m_diffuse, m_specular;

    GLfloat m_shininess;

    typedef std::vector<std::shared_ptr<vr::Texture> > TextureVector;
    TextureVector m_textures;

   public:
    Material();

    glm::vec4 getAmbient() const;
    glm::vec4 getSpecular() const;
    glm::vec4 getDiffuse() const;

    void setAmbient(const glm::vec4& color);
    void setSpecular(const glm::vec4& color);
    void setDiffuse(const glm::vec4& color);
    void setShininess(float s);

    /**
     * @brief Set the texture unit for the given texture
     *
     * @param texture The texture to set
     * @param unit The texture unit to set
     */
    void setTexture(std::shared_ptr<vr::Texture> texture, unsigned int unit);

    /**
     * @brief Apply the material to the shader
     *
     * @param shader The shader to apply the material to
     */
    void apply(std::shared_ptr<vr::Shader> shader);
};

typedef std::vector<std::shared_ptr<Material> > MaterialVector;

}  // namespace vr