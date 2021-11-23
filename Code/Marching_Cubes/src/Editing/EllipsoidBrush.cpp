#include "EllipsoidBrush.h"

EllipsoidBrush::EllipsoidBrush(glm::vec3 center, glm::vec3 radius)
:center(center),radius(radius) {

}

BrushBoundingBox EllipsoidBrush::getBoundingBox() {
    return BrushBoundingBox(
        glm::vec4(center-glm::vec3(1.1)*radius,0),
        glm::vec4(center+glm::vec3(1.1)*radius,0)
    );
}

BrushParams EllipsoidBrush::getBrushParams() {
    return {
        glm::vec4(center,0),
        glm::vec4(radius,0),

        glm::vec4(getBoundingBox().bottom,0),
        glm::vec4(getBoundingBox().top,0),

        ELLIPSOID_BRUSH,
        0, //unimplemented
        0,
        0
    };
}