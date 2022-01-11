#include "BezierBrush.h"

BezierBrush::BezierBrush(glm::vec3 A, glm::vec3 B, glm::vec3 C, float radius, BrushMode mode)
:A(A),B(B),C(C),radius(radius),mode(mode) {

}

BrushBoundingBox BezierBrush::getBoundingBox() {
    glm::vec3 min = glm::min(glm::min(A,B),C);
    glm::vec3 max = glm::max(glm::max(A,B),C);
    return BrushBoundingBox(
        glm::vec4(min-glm::vec3(radius*1.1f),0),
        glm::vec4(max+glm::vec3(radius*1.1f),0)
    );
}

BrushParams BezierBrush::getBrushParams() {
    return {
        glm::vec4(A,0),
        glm::vec4(B,0),
        glm::vec4(getBoundingBox().bottom,0),
        glm::vec4(getBoundingBox().top,0),
        BEZIER_BRUSH,
        mode,
        radius,
        0,
        glm::vec4(C,0)

    };
}