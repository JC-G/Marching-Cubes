#ifndef OCTREE_H
#define OCTREE_H

#include "GL_headers.h"
#include "MarchingChunk.h"
#include "Config.h"
#include <memory>
class Octree
{
    public:
        Octree(glm::vec3 size, glm::vec3 position, int detailLevel, GeometryGenerator* G, Octree* parent = NULL, glm::uvec3 positionInParent = glm::uvec3(0));
        virtual ~Octree();
        void update(glm::vec3 inPos);
        void draw(GLuint VAO);

        unsigned int getEdgeCode();
        void generateAllChunks();
    protected:

    private:
        glm::vec3 mySize;
        glm::vec3 myPosition;
        int myDetailLevel;
        std::shared_ptr<MarchingChunk> myChunk;
        Octree* myChildren[2][2][2];
        Octree* myParent;
        glm::uvec3 myPositionInParent;
        GeometryGenerator* myGenerator;

        bool isLeaf;
        bool hasChunk = false;

        void split(glm::vec3 inPos);
        void chop(glm::vec3 inPos);

        void updateChildren(glm::vec3 inPos);

        bool shouldSplit(glm::vec3 inPos);
        bool shouldChop(glm::vec3 inPos);

        glm::vec3 getCenter();


        void generateMarchingChunk(int edgeCode);
        void deleteChildren();

        //relativePosition - if -1 then in negative direction, if 1 then in positive direction
        //only one of the components should be non-zero
        Octree* getNeighbor(glm::ivec3 relativePosition);
};

#endif // OCTREE_H
