#include <vr/Node.h>
#include <vr/glErrorUtil.h>

using namespace vr;

/// Calculate and return a bounding box for this Node based on its Mesh objects
BoundingBox Node::calculateBoundingBox() {
    BoundingBox box;

    /*
    for (auto m : m_meshes) {
        BoundingBox meshBox = m->calculateBoundingBox();
        meshBox = meshBox * this->object2world;
        box.expand(meshBox);
    }
    */
    return box;
}