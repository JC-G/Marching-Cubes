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

bool BrushBoundingBox::intersectsRay(glm::vec3 origin, glm::vec3 direction) {
    if (contains(origin)) {
        return true;
    }
    float tMin = std::numeric_limits<float>::min();
    float tMax = std::numeric_limits<float>::max();

    for (int dim = 0; dim < 3; dim++) {
        float t1 = (bottom[dim] - origin[dim])/direction[dim];
        float t2 = (top[dim] - origin[dim])/direction[dim];
        
        tMin = glm::max(tMin,glm::min(t1,t2));
        tMax = glm::min(tMax,glm::max(t1,t2));

        if (tMin > tMax) {
            return false;
        }
    }

    return true;
}

bool BrushBoundingBox::contains(glm::vec3 point) {
    return bottom.x <= point.x && point.x <= top.x
        && bottom.y <= point.y && point.y <= top.y
        && bottom.z <= point.z && point.z <= top.z;
}