#ifndef BRUSH_H
#define BRUSH_H

#include "GL_headers.h"
#include "BrushBoundingBox.h"
// Do not use an enum here - these values will be passed to GLSL - so typedef them so they are definitely just normal ints
typedef int BrushType;

const BrushType ELLIPSOID_BRUSH = 1;
const BrushType CYLINDER_BRUSH = 2;

struct BrushParams { //size is a multiple of glm::vec4 for shader padding purposes

    //location may point to any place on the SDF, as is convenient
    //for example, in a sphere this is the center, in a box this may be the lowest corner
    //similarly, size is not guaranteed to define the bounding box of the area affected by the SDF
    //it is assumed that any bounding-box based culling is performed elsewhere, facilitated by getBoundingBox()

    glm::vec4 location; //these are vec4 for padding reasons - the 4th component need not be used
    glm::vec4 size;
    
    BrushType type; //type of brush
    int mode; //eg - add, subtract, etc
    float param1;
    float param2;
};
class Brush {
    public:
        Brush();
        virtual BrushBoundingBox getBoundingBox() = 0;
        virtual BrushParams getBrushParams() = 0;
};
#endif // BRUSH_H