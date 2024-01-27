#pragma once

#include "vr/Node.h"
#include "vr/NodeVisitor.h"

namespace vr {
class Geometry : public Node {
   public:
    Geometry(const std::string& name = "Geometry", bool useVAO = true) : Node(name), m_useVAO(useVAO) {}
    Geometry(std::vector<glm::vec4>& vertices, std::vector<glm::vec3>& normals,
             std::vector<glm::vec2>& texCoords, std::vector<GLuint>& indices,
             const std::string& name = "Geometry", bool useVAO = true) : Node(name),
                                                                         m_vertices(vertices),
                                                                         m_normals(normals),
                                                                         m_texCoords(texCoords),
                                                                         m_indices(indices),
                                                                         m_useVAO(useVAO) {}
    ~Geometry();
    virtual void accept(NodeVisitor& visitor) override;
    bool initShader(std::shared_ptr<vr::Shader> shader);
    void setInitialTransform(const glm::mat4& modelMatrix);
    void resetTransform();
    void upload();
    void buildGeometry(std::vector<glm::vec4> vertices, std::vector<glm::vec3> normals,
                       std::vector<glm::vec2> texCoords, std::vector<GLuint> indices);
    void draw(std::shared_ptr<vr::Shader> shader, const glm::mat4& modelMatrix);

    virtual BoundingBox calculateBoundingBox() override;

   private:
    std::vector<glm::vec4> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_texCoords;
    std::vector<GLuint> m_indices;

    glm::mat4 m_object2world;
    glm::mat4 m_initialTransform;

    GLint m_attribute_v_coord;
    GLint m_attribute_v_normal;
    GLint m_attribute_v_texCoords;

    bool m_useVAO;
    GLuint m_vao;
    GLuint m_vbo_vertices, m_vbo_normals, m_vbo_texCoords, m_ibo_elements = 0;
};

}  // namespace vr