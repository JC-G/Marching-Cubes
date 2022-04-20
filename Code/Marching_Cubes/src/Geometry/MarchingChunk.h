#ifndef MARCHINGCHUNK_H
#define MARCHINGCHUNK_H

/**
A MarchingChunk is the most basic piece of geometry generated using the marching cubes algorithm

it has:
    a location (the corner with smallest x,y,z coordinates)
    a size (number of cubes in each direction - note the number of points in each direction is one more than this)
    a stride (distance in between points)

it obtains its geometry from a GeometryGenerator, written into vertexBuffer and normalBuffer

*/

#include <atomic>
#include "mingw.mutex.h"
#include "GeometryGenerator.h"
#include "Config.h"
#include "ChunkMesh.h"
#include <glm/gtx/intersect.hpp>
#include "Brush.h"
class ChunkMesh;
class GeometryGenerator;
class MarchingChunk {
    public:
        static MarchingChunk* createMarchingChunk(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GeometryGenerator* Generator, int edgeIndex = 0, int detailLevel = 0,const std::vector<BrushParams>& brushes = std::vector<BrushParams>());
        virtual ~MarchingChunk();
        void tryDelete();
        void draw(GLuint VAO);
        bool hasGeometry();
        void drawBoundary(GLuint VAO);

        void generateBoundary();

        float getIntersectionPoint(glm::vec3 origin, glm::vec3 direction);
        void mapGeometry();
        std::vector<glm::vec4>* getGeometryPointer();
        int getGeometrySize();
        
        static std::vector<MarchingChunk*> loadedChunks;
        bool hasPhysicsMesh;

        // std::shared_ptr<MarchingChunk> getptr() {
        //     return shared_from_this();
        // }
    protected:

    private:
        MarchingChunk(glm::vec3 chunkLocation, glm::uvec3 chunkSize, glm::vec3 chunkStride, GeometryGenerator* Generator, int edgeIndex = 0, int detailLevel = 0,const std::vector<BrushParams>& brushes = std::vector<BrushParams>());
        void attachMesh();
        glm::vec3 myLocation;
        glm::vec3 mySize;
        glm::vec3 myStride;
        GeometryGenerator* Generator;
        GLuint myGeometrySize;
        int myDetailLevel;

        GLuint vertexBuffer;
        GLuint normalBuffer;
        GLuint boundaryBuffer;

        std::vector<glm::vec4> mappedTriangles;
        std::mutex mapMutex;
        std::atomic_bool isMapped = std::atomic_bool(false);
        ChunkMesh* myMesh;
};

#endif // MARCHINGCHUNK_H
