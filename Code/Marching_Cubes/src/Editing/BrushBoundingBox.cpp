#include "BrushBoundingBox.h"

BrushBoundingBox::BrushBoundingBox(glm::vec3 bottom, glm::vec3 top)
:bottom(bottom),top(top) {

}

BrushBoundingBox BrushBoundingBox::getChunkBox(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride) {
    return BrushBoundingBox(chunkLocation,chunkLocation + glm::vec3(chunkSize) * chunkStride);
}

bool BrushBoundingBox::intersects(BrushBoundingBox other) {
    //AABB intersection
    return ((bottom.x <= other.bottom.x && other.bottom.x <= top.x) ||
           (other.bottom.x <= bottom.x && bottom.x <= other.top.x)) &&
           ((bottom.y <= other.bottom.y && other.bottom.y <= top.y) ||
           (other.bottom.y <= bottom.y && bottom.y <= other.top.y)) &&
           ((bottom.z <= other.bottom.z && other.bottom.z <= top.z) ||
           (other.bottom.z <= bottom.z && bottom.z <= other.top.z));

}