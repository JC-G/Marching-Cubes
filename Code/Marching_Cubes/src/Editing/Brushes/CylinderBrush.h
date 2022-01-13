#ifndef CYLINDERBRUSH_H
#define CYLINDERBRUSH_H
#include "Brush.h"
class CylinderBrush : public Brush {
    public:
        CylinderBrush(glm::vec3 a, glm::vec3 b, float r);
        BrushBoundingBox getBoundingBox() override;
        BrushParams getBrushParams() override;
    private:
        glm::vec3 a;
        glm::vec3 b;
        float r;
};
#endif