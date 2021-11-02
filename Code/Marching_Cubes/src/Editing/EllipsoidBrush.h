#ifndef ELLIPSOIDBRUSH_H
#define ELLIPSOIDBRUSH_H
#include "Brush.h"
class EllipsoidBrush : public Brush {
    public:
        EllipsoidBrush(glm::vec3 center, glm::vec3 radius);
        BrushBoundingBox getBoundingBox() override;
        BrushParams getBrushParams() override;
    private:
        glm::vec3 center;
        glm::vec3 radius;
};

#endif //ELLIPSOIDBRUSH_H