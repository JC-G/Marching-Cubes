#ifndef OCTREE_H
#define OCTREE_H

#include "GL_headers.h"
#include "MarchingChunk.h"

#include <memory>
class Octree
{
    public:
        Octree(glm::vec3 size, glm::vec3 position, int detailLevel, GeometryGenerator* G);
        virtual ~Octree();
        void update(glm::vec3 inPos);
        void draw(GLuint VAO);
    protected:

    private:
        glm::vec3 mySize;
        glm::vec3 myPosition;
        int myDetailLevel;
        std::shared_ptr<MarchingChunk> myChunk;
        Octree* myChildren[2][2][2];
        GeometryGenerator* myGenerator;

        bool isLeaf;
        bool hasChunk = false;

        void split(glm::vec3 inPos);
        void chop(glm::vec3 inPos);

        void updateChildren(glm::vec3 inPos);

        bool shouldSplit(glm::vec3 inPos);
        bool shouldChop(glm::vec3 inPos);

        glm::vec3 getCenter();

        void generateMarchingChunk();
        void deleteChildren();
};

#endif // OCTREE_H
