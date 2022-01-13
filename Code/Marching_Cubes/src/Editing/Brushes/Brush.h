#ifndef BRUSH_H
#define BRUSH_H

#include "GL_headers.h"
#include "BrushBoundingBox.h"
// Do not use an enum here - these values will be passed to GLSL - so typedef them so they are definitely just normal ints
typedef int BrushType;

const BrushType ELLIPSOID_BRUSH = 1;
const BrushType CYLINDER_BRUSH = 2;
const BrushType BEZIER_BRUSH = 3;
const BrushType CUBIC_BEZIER_BRUSH = 4;


typedef int BrushMode;

const BrushMode MODE_ADD = 0;
const BrushMode MODE_DELETE = 1;


struct BrushParams { //size is a multiple of glm::vec4 for shader padding purposes

    //location may point to any place on the SDF, as is convenient
    //for example, in a sphere this is the center, in a box this may be the lowest corner
    //similarly, size is not guaranteed to define the bounding box of the area affected by the SDF
    //it is assumed that any bounding-box based culling is performed elsewhere, facilitated by getBoundingBox()

    //it is also assumed that the SDF of the brush will have NO AFFECT on the underlying terrain function outside of the bounding box
    //this is enforced in terrain_modification.glsl
    //In the case where a bounding box is exact, this means a small amount of padding should be used since the SDF has an effect on the boundary of the box
    //See ellipsoid brush, which has a simple bounding box

  
    glm::vec4 location; //these are vec4 for padding reasons - the 4th component need not be used
    glm::vec4 size;

    glm::vec4 bottom; //for bounding box
    glm::vec4 top;
    
    BrushType type; //type of brush
    BrushMode mode; //eg - add, subtract, etc
    float param1;
    float param2;
    glm::vec4 data1;
    glm::vec4 data2;
};
class Brush {
    public:
        Brush();
        virtual BrushBoundingBox getBoundingBox() = 0;
        virtual BrushParams getBrushParams() = 0;
};
#endif // BRUSH_H