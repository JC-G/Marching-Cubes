#ifndef ELLIPSOIDBRUSH_H
#define ELLIPSOIDBRUSH_H
#include "Brush.h"
class EllipsoidBrush : public Brush {
    public:
        EllipsoidBrush(glm::vec3 center, glm::vec3 radius, BrushMode mode = 0);
        BrushBoundingBox getBoundingBox() override;
        BrushParams getBrushParams() override;
    private:
        glm::vec3 center;
        glm::vec3 radius;
        BrushMode mode;
};

#endif //ELLIPSOIDBRUSH_H