#ifndef BRUSHBOUNDINGBOX_H
#define BRUSHBOUNDINGBOX_H
#include "GL_headers.h"
class BrushBoundingBox {
    public:
        BrushBoundingBox(glm::vec3 bottom, glm::vec3 top);
        glm::vec3 bottom; //-x,-y,-z corner
        glm::vec3 top; //+x,+y,+z corner    
    private:

};
#endif //BRUSHBOUNDINGBOX_