#ifndef CUBICBEZIERBRUSH_H
#define CUBICBEZIERBRUSH_H
#include "Brush.h"
class CubicBezierBrush : public Brush {
    public:
        CubicBezierBrush(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, float radius, BrushMode mode = 0);
        BrushBoundingBox getBoundingBox() override;
        BrushParams getBrushParams() override;
        
    private:
        glm::vec3 A;
        glm::vec3 B;
        glm::vec3 C;
        glm::vec3 D;
        float radius;
        BrushMode mode;
};
#endif