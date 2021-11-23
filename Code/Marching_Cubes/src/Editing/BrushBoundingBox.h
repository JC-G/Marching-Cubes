#ifndef BRUSHBOUNDINGBOX_H
#define BRUSHBOUNDINGBOX_H
#include "GL_headers.h"
#include <iostream>
class BrushBoundingBox {
    public:
        BrushBoundingBox(glm::vec3 bottom, glm::vec3 top);
        glm::vec3 bottom; //-x,-y,-z corner
        glm::vec3 top; //+x,+y,+z corner  

        static BrushBoundingBox getChunkBox(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride);

        bool intersects(BrushBoundingBox other);
        bool intersectsRay(glm::vec3 origin, glm::vec3 direction);
        bool contains(glm::vec3 point);
    private:

};
#endif //BRUSHBOUNDINGBOX_