#include "SubMesh.hpp"

SubMesh::SubMesh(std::string name, int vertexCount, int faceCount) {
    this->name = name;
    this->numFaces = faceCount;
    this->numVertices = vertexCount;
}

std::string SubMesh::GetName() const {
    return name;
}

int SubMesh::GetFaceCount() const {
    return numFaces;
}

int SubMesh::GetVertexCount() const {
    return numVertices;
}
