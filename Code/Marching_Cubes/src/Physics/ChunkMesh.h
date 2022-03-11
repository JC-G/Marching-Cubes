#ifndef CHUNKMESH_H
#define CHUNKMESH_H

#include <btBulletDynamicsCommon.h>

#include "MarchingChunk.h"

#include "LockQueue.h"

const int CHUNKMESH_INITIALIZED   = 1;
const int CHUNKMESH_GENERATING    = 5;
const int CHUNKMESH_FUTURE_DELETE = 7;
const int CHUNKMESH_GENERATED     =10;
const int CHUNKMESH_INWORLD       =20;
const int CHUNKMESH_REMOVING      =25;
const int CHUNKMESH_REMOVED       =30;

class MarchingChunk;
class ChunkMesh {
    public:
        static ChunkMesh* CreateMesh(MarchingChunk* chunk);
        ~ChunkMesh();

        //multithreading
        //represent the state of the mesh
        std::atomic_int state;
        void addToWorld();
        void generateMesh();
        void removeFromWorld();
        void cleanUp();
        void tryDelete();
        
        
        MarchingChunk* myChunk;


    private:
        ChunkMesh(MarchingChunk* chunk);
        btBvhTriangleMeshShape* myShape;
        btTriangleIndexVertexArray* meshInterface;
        btDefaultMotionState* myMotionState;
        btRigidBody* body;


        //hack for non-indexed mesh
        static std::vector<int> indices;
        static int maxIndex;
        static void resizeIndices(int maxVal);

};

#endif