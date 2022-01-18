#ifndef ROADBRUSH_H
#define ROADBRUSH_H

#include "Brush.h"
class RoadBrush : public Brush {
    public:
        RoadBrush(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, float radius, BrushMode mode = 0);
        BrushBoundingBox getBoundingBox() override;
        BrushParams getBrushParams() override;
        
    private:
        glm::vec3 A;
        glm::vec3 B;
        glm::vec3 C;
        glm::vec3 D;
        float radius;
        BrushMode mode;

        //TODO - make this into another vector of parameters
        float H1=0.1;
        float H2=-1.0;
        float K1=0.4;
        float K2=-4.0;
};
#endif