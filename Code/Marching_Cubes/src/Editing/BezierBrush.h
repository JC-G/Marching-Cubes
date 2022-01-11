#ifndef BEZIERBRUSH_H
#define BEZIERBRUSH_H
#include "Brush.h"
class BezierBrush : public Brush {
    public:
        BezierBrush(glm::vec3 A, glm::vec3 B, glm::vec3 C, float radius, BrushMode mode = 0);
        BrushBoundingBox getBoundingBox() override;
        BrushParams getBrushParams() override;
    private:
        glm::vec3 A;
        glm::vec3 B;
        glm::vec3 C;
        float radius;
        BrushMode mode;
};
#endif