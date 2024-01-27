#pragma once

#include "vr/Geometry.h"

#include <vr/glErrorUtil.h>

using namespace vr;

Geometry::~Geometry() {
    if (m_vbo_vertices != 0)
        glDeleteBuffers(1, &m_vbo_vertices);

    if (m_vbo_normals != 0)
        glDeleteBuffers(1, &m_vbo_normals);

    if (m_ibo_elements != 0)
        glDeleteBuffers(1, &m_ibo_elements);
}

void Geometry::accept(NodeVisitor& visitor) {
    std::cerr << "Accept: " << m_name << std::endl;
    visitor.visit(this);
}

bool Geometry::initShader(std::shared_ptr<vr::Shader> shader) {
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

void Geometry::draw(std::shared_ptr<vr::Shader> shader, const glm::mat4& modelMatrix) {
    shader->use();
    CHECK_GL_ERROR_LINE_FILE();
    if (m_useVAO) {
        glBindVertexArray(m_vao);
        CHECK_GL_ERROR_LINE_FILE();
    }

    CHECK_GL_ERROR_LINE_FILE();

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
        CHECK_GL_ERROR_LINE_FILE();
    }
    CHECK_GL_ERROR_LINE_FILE();

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

        glDrawElements(GL_TRIANGLES, (GLsizei)this->m_indices.size(), GL_UNSIGNED_INT, 0);
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

    if (m_useVAO)
        glBindVertexArray(0);
}

BoundingBox Geometry::calculateBoundingBox() {
    BoundingBox box;
    for (auto v : m_vertices) {
        glm::vec3 vTransformed = m_object2world * v;
        box.expand(vTransformed);
    }
    return box;
}