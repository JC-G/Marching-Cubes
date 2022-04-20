#ifndef OCTREE_H
#define OCTREE_H

#include "GL_headers.h"
#include "MarchingChunk.h"
#include "Config.h"
#include <memory>
#include "Editing.h"
#include "BrushBoundingBox.h"
#include "Timing.h"
class GeometryGenerator;
class MarchingChunk;
class Octree
{
    public:
        Octree(glm::vec3 size, glm::vec3 position, int detailLevel, GeometryGenerator* G, Octree* parent = NULL, glm::uvec3 positionInParent = glm::uvec3(0));
        virtual ~Octree();
        void draw(GLuint VAO);
        void drawBoundary(GLuint VAO);
        void refresh(glm::vec3 inPos);
        unsigned int getEdgeIndex();
        void generateAllChunks(bool force = false);

        BrushBoundingBox getBoundingBox();

        float getIntersectionPoint(glm::vec3 origin, glm::vec3 direction);
        Octree* child(int i, int j, int k);
        void insertBrush(Brush* b);
    protected:

    private:
        glm::vec3 mySize;
        glm::vec3 myPosition;
        int myDetailLevel;
        MarchingChunk* myChunk;
        Octree* myChildren[2][2][2];
        Octree* childFromVec3(glm::ivec3 pos);
        GeometryGenerator* myGenerator;
        Octree* myParent;
        glm::uvec3 myPositionInParent;

        bool isLeaf;
        bool hasChunk = false;

        void split();
        void chop();

        bool refine();

        bool shouldSplit(glm::vec3 inPos);
        bool shouldChop(glm::vec3 inPos);

        glm::vec3 getCenter();


        void generateMarchingChunk(int edgeIndex);
        void deleteChildren();

        //relativePosition - if -1 then in negative direction, if 1 then in positive direction
        //only one of the components should be non-zero
        Octree* getNeighbor(glm::ivec3 relativePosition);

        unsigned int edgeIndex = 0;

        bool needsRegen = false;

        //Octree updating 2.0
        //flagged chunks are deleted
        bool flagged = false;
        bool flagSplitPhase(glm::vec3 inPos);
        // void splitPhase(glm::vec3 inPos);
        void deleteRegenPhase();

        static Timing chunkTimer;

        std::vector<Brush*> myBrushes;

        // std::vector<Brush*> getAllBrushesBelow();
        // std::vector<BrushParams> getRenderedBrushes();

};

#endif // OCTREE_H
