#include "CubicBezierBrush.h"

CubicBezierBrush::CubicBezierBrush(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, float radius, BrushMode mode) 
:A(A),B(B),C(C),D(D),radius(radius),mode(mode) {

}

BrushBoundingBox CubicBezierBrush::getBoundingBox() {
    glm::vec3 min = glm::min(glm::min(A,B),glm::min(C,D));
    glm::vec3 max = glm::max(glm::max(A,B),glm::max(C,D));
    return BrushBoundingBox(
        glm::vec4(min-glm::vec3(radius*1.1f),0),
        glm::vec4(max+glm::vec3(radius*1.1f),0)
    );
}

BrushParams CubicBezierBrush::getBrushParams() {
    return {
        glm::vec4(A,0),
        glm::vec4(B,0),
        glm::vec4(getBoundingBox().bottom,0),
        glm::vec4(getBoundingBox().top,0),
        CUBIC_BEZIER_BRUSH,
        mode,
        radius,
        0,
        glm::vec4(C,0),
        glm::vec4(D,0),

    };
}