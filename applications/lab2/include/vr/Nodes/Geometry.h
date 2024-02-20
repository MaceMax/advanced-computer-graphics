#pragma once

#include "Node.h"
#include "vr/Visitors/NodeVisitor.h"

/**
 * A Geometry node represents a 3D object in the scene graph.
 */

namespace vr {
class Geometry : public Node {
   public:
    /**
     * @brief Constructs a new Geometry object
     *
     * @param name  The name of the geometry
     * @param useVAO Whether to use VAOs or not
     */
    Geometry(const std::string& name = "Geometry", bool useVAO = true) : Node(name), m_useVAO(useVAO) {}

    /**
     * @brief Constructs a new Geometry object with the given vertices, normals, texture coordinates and indices
     *
     * @param vertices The vertices of the geometry
     * @param normals  The normals of the geometry
     * @param texCoords The texture coordinates of the geometry
     * @param indices The indices of the geometry
     * @param name The name of the geometry
     * @param useVAO Whether to use VAOs or not
     */
    Geometry(std::vector<glm::vec4> vertices, std::vector<glm::vec3> normals,
             std::vector<glm::vec2> texCoords, std::vector<glm::vec3> tangents,
             std::vector<glm::vec3> bitangents, std::vector<GLuint> indices,
             const std::string& name = "Geometry", bool useVAO = true) : Node(name),
                                                                         m_vertices(vertices),
                                                                         m_normals(normals),
                                                                         m_texCoords(texCoords),
                                                                         m_tangents(tangents),
                                                                         m_bitangents(bitangents),
                                                                         m_indices(indices),
                                                                         m_useVAO(useVAO) {}
    ~Geometry();

    virtual void accept(NodeVisitor& visitor) override;

    /**
     * @brief Initializes the shader uniforms
     *
     * @param shader The shader to initialize
     * @return true if the shader was initialized successfully, false otherwise
     */
    bool initShader(const std::shared_ptr<vr::Shader>& shader);

    /**
     * @brief Sets the initial transform of the geometry
     *
     * @param modelMatrix The initial transform
     */
    void setInitialTransform(const glm::mat4& modelMatrix);

    /**
     * @brief Resets the transform of the geometry
     */
    void resetTransform();

    /**
     * @brief Uploads the geometry to the GPU
     */
    void upload();

    /**
     * @brief Builds the geometry from the given vertices, normals, texture coordinates and indices
     *
     * @param vertices The vertices of the geometry
     * @param normals  The normals of the geometry
     * @param texCoords The texture coordinates of the geometry
     * @param indices The indices of the geometry
     */
    void buildGeometry(std::vector<glm::vec4> vertices, std::vector<glm::vec3> normals,
                       std::vector<glm::vec2> texCoords, std::vector<GLuint> indices);

    /**
     * @brief Draws the geometry
     *
     * @param shader The shader to use
     * @param modelMatrix  The model matrix
     */
    void draw(std::shared_ptr<vr::Shader> const& shader, const glm::mat4& modelMatrix);

    /**
     * @brief Draws the bounding box of the geometry
     *
     * @param shader The shader to use
     */
    void draw_bbox(std::shared_ptr<vr::Shader> shader);

    /**
     * @brief Calculates the bounding box of the geometry
     *
     * @param t_mat The transformation matrix
     * @return The bounding box
     */
    virtual BoundingBox calculateBoundingBox(glm::mat4 t_mat) override;

   private:
    std::vector<glm::vec4> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_texCoords;
    std::vector<glm::vec3> m_tangents;
    std::vector<glm::vec3> m_bitangents;

    std::vector<GLuint> m_indices;

    glm::mat4 m_object2world;
    glm::mat4 m_initialTransform;

    GLint m_attribute_v_coord = -1;
    GLint m_attribute_v_normal = -1;
    GLint m_attribute_v_texCoords = -1;
    GLint m_attribute_v_tangent = -1;
    GLint m_attribute_v_bitangent = -1;

    bool m_useVAO;
    GLuint m_vao;
    GLuint m_vbo_vertices = 0, m_vbo_normals = 0, m_vbo_texCoords = 0, m_ibo_elements = 0, m_vbo_tangents = 0, m_vbo_bitangents = 0;
};

}  // namespace vr