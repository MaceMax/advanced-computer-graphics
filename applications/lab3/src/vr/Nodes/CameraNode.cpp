#include <vr/Nodes/CameraNode.h>

using namespace vr;

void CameraNode::accept(NodeVisitor& visitor) {
    visitor.visit(this);
}