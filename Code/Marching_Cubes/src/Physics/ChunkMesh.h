#ifndef CHUNKMESH_H
#define CHUNKMESH_H

#include <btBulletDynamicsCommon.h>

#include "MarchingChunk.h"

class MarchingChunk;
class ChunkMesh {
    public:
        ChunkMesh(MarchingChunk* chunk);
        ~ChunkMesh();
    private:
        btBvhTriangleMeshShape* myShape;
        // MarchingChunk* myChunk;
        btTriangleIndexVertexArray* meshInterface;
        btDefaultMotionState* myMotionState;
        btRigidBody* body;

        //hack for non-indexed mesh
        static std::vector<int> indices;
        static int maxIndex;
        static void resizeIndices(int maxVal);
};

#endif