#include "RoadBrush.h"

RoadBrush::RoadBrush(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, float radius, BrushMode mode) 
:A(A),B(B),C(C),D(D),radius(radius),mode(mode) {

}

BrushBoundingBox RoadBrush::getBoundingBox() {
    glm::vec3 min = glm::min(glm::min(A,B),glm::min(C,D));
    glm::vec3 max = glm::max(glm::max(A,B),glm::max(C,D));

    return BrushBoundingBox(
        glm::vec4(min - glm::vec3(radius),0),
        glm::vec4(max+glm::vec3(radius),0)
    );
    float roadOffset = radius - (H2-H1)*(K2*K1)/(K2-K1);
    glm::vec3 offsetTop = glm::vec3(
        radius+roadOffset,
        H1,
        radius+roadOffset
    );
    glm::vec3 offsetBottom = glm::vec3(
        radius+roadOffset,
        -H2,
        radius+roadOffset

    );
    return BrushBoundingBox(
        glm::vec4(min-offsetBottom*1.1f,0),
        glm::vec4(max+offsetTop*1.1f,0)
    );
}

BrushParams RoadBrush::getBrushParams() {
    return {
        glm::vec4(A,0),
        glm::vec4(B,0),
        glm::vec4(getBoundingBox().bottom,0),
        glm::vec4(getBoundingBox().top,0),
        ROAD_BRUSH,
        mode,
        radius,
        0,
        glm::vec4(C,0),
        glm::vec4(D,0),

    };
}