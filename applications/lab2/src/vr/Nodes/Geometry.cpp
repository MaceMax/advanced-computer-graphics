#include "vr/Nodes/Geometry.h"

#include <vr/glErrorUtil.h>

#include <glm/gtx/transform.hpp>

using namespace vr;

Geometry::~Geometry() {
    if (m_useVAO && m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }

    if (m_vbo_vertices != 0) {
        glDeleteBuffers(1, &m_vbo_vertices);
        m_vbo_vertices = 0;
    }

    if (m_vbo_normals != 0) {
        glDeleteBuffers(1, &m_vbo_normals);
        m_vbo_normals = 0;
    }

    if (m_vbo_texCoords != 0) {
        glDeleteBuffers(1, &m_vbo_texCoords);
        m_vbo_texCoords = 0;
    }

    if (m_ibo_elements != 0) {
        glDeleteBuffers(1, &m_ibo_elements);
        m_ibo_elements = 0;
    }

    if (m_vbo_tangents != 0) {
        glDeleteBuffers(1, &m_vbo_tangents);
        m_vbo_tangents = 0;
    }

    if (m_vbo_bitangents != 0) {
        glDeleteBuffers(1, &m_vbo_bitangents);
        m_vbo_bitangents = 0;
    }
}

void Geometry::accept(NodeVisitor& visitor) {
    visitor.visit(this);
}

bool Geometry::initShader(const std::shared_ptr<vr::Shader>& shader) {
    shader->use();

    const char* attributeName;
    attributeName = "vertex_position";
    m_attribute_v_coord = shader->getAttribute(attributeName);
    if (m_attribute_v_coord == -1)
        return false;

    attributeName = "vertex_normal";
    m_attribute_v_normal = shader->getAttribute(attributeName);
    if (m_attribute_v_normal == -1)
        return false;

    attributeName = "vertex_texCoord";
    m_attribute_v_texCoords = shader->getAttribute(attributeName);
    if (m_attribute_v_texCoords == -1)
        return false;

    attributeName = "vertex_tangent";
    m_attribute_v_tangent = shader->getAttribute(attributeName);
    if (m_attribute_v_tangent == -1)
        return false;

    attributeName = "vertex_bitangent";
    m_attribute_v_bitangent = shader->getAttribute(attributeName);
    if (m_attribute_v_bitangent == -1)
        return false;

    return true;
}

void Geometry::upload() {
    if (m_useVAO) {
        // Create a Vertex Array Object that will handle all VBO:s of this Mesh
        glGenVertexArrays(1, &m_vao);
        CHECK_GL_ERROR_LINE_FILE();
        glBindVertexArray(m_vao);
        CHECK_GL_ERROR_LINE_FILE();
    }

    if (m_vertices.size() > 0) {
        glGenBuffers(1, &this->m_vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(m_vertices[0]),
                     m_vertices.data(), GL_STATIC_DRAW);
        CHECK_GL_ERROR_LINE_FILE();
    }

    if (this->m_normals.size() > 0) {
        glGenBuffers(1, &this->m_vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, this->m_normals.size() * sizeof(this->m_normals[0]),
                     this->m_normals.data(), GL_STATIC_DRAW);
        CHECK_GL_ERROR_LINE_FILE();
    }

    if (this->m_texCoords.size() > 0) {
        glGenBuffers(1, &this->m_vbo_texCoords);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_texCoords);
        glBufferData(GL_ARRAY_BUFFER, this->m_texCoords.size() * sizeof(this->m_texCoords[0]),
                     this->m_texCoords.data(), GL_STATIC_DRAW);
        CHECK_GL_ERROR_LINE_FILE();
    }

    if (this->m_tangents.size() > 0) {
        glGenBuffers(1, &this->m_vbo_tangents);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_tangents);
        glBufferData(GL_ARRAY_BUFFER, this->m_tangents.size() * sizeof(this->m_tangents[0]),
                     this->m_tangents.data(), GL_STATIC_DRAW);
        CHECK_GL_ERROR_LINE_FILE();
    }

    if (this->m_bitangents.size() > 0) {
        glGenBuffers(1, &this->m_vbo_bitangents);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_bitangents);
        glBufferData(GL_ARRAY_BUFFER, this->m_bitangents.size() * sizeof(this->m_bitangents[0]),
                     this->m_bitangents.data(), GL_STATIC_DRAW);
        CHECK_GL_ERROR_LINE_FILE();
    }

    if (m_useVAO) {
        if (this->m_vbo_vertices != 0) {
            glEnableVertexAttribArray(m_attribute_v_coord);
            glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_vertices);
            glVertexAttribPointer(
                m_attribute_v_coord,  // attribute
                4,                    // number of elements per vertex, here (x,y,z,w)
                GL_FLOAT,             // the type of each element
                GL_FALSE,             // take our values as-is
                0,                    // no extra data between each position
                0                     // offset of first element
            );
            glDisableVertexAttribArray(m_attribute_v_coord);
        }

        if (this->m_vbo_normals != 0) {
            glEnableVertexAttribArray(m_attribute_v_normal);
            glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_normals);
            glVertexAttribPointer(
                m_attribute_v_normal,  // attribute
                3,                     // number of elements per vertex, here (x,y,z)
                GL_FLOAT,              // the type of each element
                GL_FALSE,              // take our values as-is
                0,                     // no extra data between each position
                0                      // offset of first element
            );
            glDisableVertexAttribArray(m_attribute_v_normal);
        }

        if (this->m_vbo_texCoords != 0) {
            glEnableVertexAttribArray(m_attribute_v_texCoords);
            glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_texCoords);
            glVertexAttribPointer(
                m_attribute_v_texCoords,  // attribute
                2,                        // number of elements per vertex, here (x,y)
                GL_FLOAT,                 // the type of each element
                GL_FALSE,                 // take our values as-is
                0,                        // no extra data between each position
                0                         // offset of first element
            );
            glDisableVertexAttribArray(m_attribute_v_texCoords);
        }

        if (this->m_vbo_tangents != 0) {
            glEnableVertexAttribArray(m_attribute_v_tangent);
            glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_tangents);
            glVertexAttribPointer(
                m_attribute_v_tangent,  // attribute
                3,                      // number of elements per vertex, here (x,y,z)
                GL_FLOAT,               // the type of each element
                GL_FALSE,               // take our values as-is
                0,                      // no extra data between each position
                0                       // offset of first element
            );
            glDisableVertexAttribArray(m_attribute_v_tangent);
        }

        if (this->m_vbo_bitangents != 0) {
            glEnableVertexAttribArray(m_attribute_v_bitangent);
            glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_bitangents);
            glVertexAttribPointer(
                m_attribute_v_bitangent,  // attribute
                3,                        // number of elements per vertex, here (x,y,z)
                GL_FLOAT,                 // the type of each element
                GL_FALSE,                 // take our values as-is
                0,                        // no extra data between each position
                0                         // offset of first element
            );
            glDisableVertexAttribArray(m_attribute_v_bitangent);
        }
    }

    if (this->m_indices.size() > 0) {
        glGenBuffers(1, &this->m_ibo_elements);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ibo_elements);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_indices.size() * sizeof(this->m_indices[0]),
                     this->m_indices.data(), GL_STATIC_DRAW);
    }

    CHECK_GL_ERROR_LINE_FILE();

    if (m_useVAO) {
        // Now release VAO
        glEnableVertexAttribArray(0);  // Disable our Vertex Array Object
        glBindVertexArray(0);          // Disable our Vertex Buffer Object
        CHECK_GL_ERROR_LINE_FILE();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void Geometry::setInitialTransform(const glm::mat4& modelMatrix) {
    m_object2world = m_initialTransform = modelMatrix;
}

void Geometry::resetTransform() {
    m_object2world = m_initialTransform;
}

void Geometry::buildGeometry(std::vector<glm::vec4> vertices, std::vector<glm::vec3> normals,
                             std::vector<glm::vec2> texCoords, std::vector<GLuint> indices) {
    m_vertices = vertices;
    m_normals = normals;
    m_texCoords = texCoords;
    m_indices = indices;
}

void Geometry::draw(std::shared_ptr<vr::Shader> const& shader, const glm::mat4& modelMatrix) {
    if (m_useVAO) {
        glBindVertexArray(m_vao);
        CHECK_GL_ERROR_LINE_FILE();
    }

    if (m_normals.size() == 0) {
        draw_bbox(shader);
        return;
    }

    if (!m_useVAO) {
        if (this->m_vbo_vertices != 0) {
            glEnableVertexAttribArray(m_attribute_v_coord);
            glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_vertices);
            glVertexAttribPointer(
                m_attribute_v_coord,  // attribute
                4,                    // number of elements per vertex, here (x,y,z,w)
                GL_FLOAT,             // the type of each element
                GL_FALSE,             // take our values as-is
                0,                    // no extra data between each position
                0                     // offset of first element
            );
        }

        if (this->m_vbo_normals != 0) {
            glEnableVertexAttribArray(m_attribute_v_normal);
            glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_normals);
            glVertexAttribPointer(
                m_attribute_v_normal,  // attribute
                3,                     // number of elements per vertex, here (x,y,z)
                GL_FLOAT,              // the type of each element
                GL_FALSE,              // take our values as-is
                0,                     // no extra data between each position
                0                      // offset of first element
            );
        }
        if (this->m_vbo_texCoords != 0) {
            glEnableVertexAttribArray(m_attribute_v_texCoords);
            glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_texCoords);
            glVertexAttribPointer(
                m_attribute_v_texCoords,  // attribute
                2,                        // number of elements per vertex, here (x,y,z)
                GL_FLOAT,                 // the type of each element
                GL_FALSE,                 // take our values as-is
                0,                        // no extra data between each position
                0                         // offset of first element
            );
        }
    } else {
        glEnableVertexAttribArray(m_attribute_v_coord);
        CHECK_GL_ERROR_LINE_FILE();
        glEnableVertexAttribArray(m_attribute_v_normal);
        CHECK_GL_ERROR_LINE_FILE();
        if (m_vbo_texCoords != 0)
            glEnableVertexAttribArray(m_attribute_v_texCoords);

        if (m_vbo_tangents != 0)
            glEnableVertexAttribArray(m_attribute_v_tangent);

        if (m_vbo_bitangents != 0)
            glEnableVertexAttribArray(m_attribute_v_bitangent);
        CHECK_GL_ERROR_LINE_FILE();
    }

    /* Apply object's transformation matrix */
    glm::mat4 obj2World = modelMatrix * m_object2world;

    shader->setMat4("m", obj2World);
    /*
    Transform normal vectors with transpose of inverse of upper left
    3x3 model matrix (ex-gl_NormalMatrix):
    */
    glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(obj2World)));
    shader->setMat3("m_3x3_inv_transp", m_3x3_inv_transp);
    /* Push each element in buffer_vertices to the vertex shader */
    if (this->m_ibo_elements != 0) {
        if (!m_useVAO)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ibo_elements);
        GLuint size = GLuint(this->m_indices.size());
        glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
        CHECK_GL_ERROR_LINE_FILE();
    } else {
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->m_vertices.size());
    }

    if (this->m_vbo_normals != 0)
        glDisableVertexAttribArray(m_attribute_v_normal);

    if (this->m_vbo_vertices != 0)
        glDisableVertexAttribArray(m_attribute_v_coord);

    if (this->m_vbo_texCoords != 0)
        glDisableVertexAttribArray(m_attribute_v_texCoords);

    if (this->m_vbo_tangents != 0)
        glDisableVertexAttribArray(m_attribute_v_tangent);

    if (this->m_vbo_bitangents != 0)
        glDisableVertexAttribArray(m_attribute_v_bitangent);

    if (m_useVAO)
        glBindVertexArray(0);
}

void Geometry::draw_bbox(std::shared_ptr<vr::Shader> shader) {
    shader->use();
    if (this->m_vertices.size() == 0)
        return;

    // Cube 1x1x1, centered on origin
    GLfloat vertices[] = {
        -0.5,
        -0.5,
        -0.5,
        1.0,
        0.5,
        -0.5,
        -0.5,
        1.0,
        0.5,
        0.5,
        -0.5,
        1.0,
        -0.5,
        0.5,
        -0.5,
        1.0,
        -0.5,
        -0.5,
        0.5,
        1.0,
        0.5,
        -0.5,
        0.5,
        1.0,
        0.5,
        0.5,
        0.5,
        1.0,
        -0.5,
        0.5,
        0.5,
        1.0,
    };

    GLuint vbo_vertices;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLushort elements[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7};

    GLuint ibo_elements;
    glGenBuffers(1, &ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GLfloat
        min_x,
        max_x,
        min_y, max_y,
        min_z, max_z;

    min_x = max_x = this->m_vertices[0].x;
    min_y = max_y = this->m_vertices[0].y;
    min_z = max_z = this->m_vertices[0].z;
    for (unsigned int i = 0; i < this->m_vertices.size(); i++) {
        if (this->m_vertices[i].x < min_x) min_x = this->m_vertices[i].x;
        if (this->m_vertices[i].x > max_x) max_x = this->m_vertices[i].x;
        if (this->m_vertices[i].y < min_y) min_y = this->m_vertices[i].y;
        if (this->m_vertices[i].y > max_y) max_y = this->m_vertices[i].y;
        if (this->m_vertices[i].z < min_z) min_z = this->m_vertices[i].z;
        if (this->m_vertices[i].z > max_z) max_z = this->m_vertices[i].z;
    }

    glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
    glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
    glm::mat4 transform = glm::scale(glm::mat4(1), size) * glm::translate(glm::mat4(1), center);

    /* Apply object's transformation matrix */
    glm::mat4 m = this->m_object2world * transform;
    shader->setMat4("m", m);

    CHECK_GL_ERROR_LINE_FILE();

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glEnableVertexAttribArray(m_attribute_v_coord);
    glVertexAttribPointer(
        m_attribute_v_coord,  // attribute
        4,                    // number of elements per vertex, here (x,y,z,w)
        GL_FLOAT,             // the type of each element
        GL_FALSE,             // take our values as-is
        0,                    // no extra data between each position
        0                     // offset of first element
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(m_attribute_v_coord);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &vbo_vertices);
    glDeleteBuffers(1, &ibo_elements);
    CHECK_GL_ERROR_LINE_FILE();
}

BoundingBox Geometry::calculateBoundingBox(glm::mat4 t_mat) {
    BoundingBox box;
    for (auto v : m_vertices) {
        glm::vec3 vTransformed = t_mat * v;
        box.expand(vTransformed);
    }
    return box;
}