#include "CylinderBrush.h"

CylinderBrush::CylinderBrush(glm::vec3 a, glm::vec3 b, float r,BrushMode mode) 
:a(a),b(b),r(r),mode(mode){

}
BrushBoundingBox CylinderBrush::getBoundingBox() {
    return BrushBoundingBox(
        glm::vec4(glm::min(a,b)-glm::vec3(r),0),
        glm::vec4(glm::max(a,b)+glm::vec3(r),0)
    );
}
BrushParams CylinderBrush::getBrushParams() {
    return {
        glm::vec4(a,0),
        glm::vec4(b,0),

        glm::vec4(getBoundingBox().bottom,0),
        glm::vec4(getBoundingBox().top,0),

        CYLINDER_BRUSH,
        mode,
        r,
        0
    };
}