#include "ChunkMesh.h"

#include "GL_headers.h"
#include "PhysicsWorld.h"
#include "MeshManager.h"

int ChunkMesh::maxIndex = 0;
std::vector<int> ChunkMesh::indices;

ChunkMesh::ChunkMesh(MarchingChunk* chunk) 
:myChunk(chunk)
{
    // std::cout << "Created " << this << " with: " << myChunk << std::endl;
    state.store(CHUNKMESH_INITIALIZED);
    myChunk->mapGeometry();
    //indices will be at least as big as needed
    resizeIndices(myChunk->getGeometrySize());


}
ChunkMesh* ChunkMesh::CreateMesh(MarchingChunk* chunk) {
    ChunkMesh* C = new ChunkMesh(chunk);
    MeshManager::manager->addTask(C);
    return C;

}

ChunkMesh::~ChunkMesh() {

    //Delete the marchingchunk this relied on:
    delete myChunk;

}

void ChunkMesh::resizeIndices(int maxVal) {
    for (int i = maxIndex; i < maxVal; i++) {
        indices.push_back(i);
    }
    maxIndex = maxVal;
}

void ChunkMesh::removeFromWorld() {
    //deletion order matters here?
    delete body->getMotionState();
    delete body->getCollisionShape();
    PhysicsWorld::removeRigidBody(body);    
    delete body;
    delete meshInterface;
}

void ChunkMesh::generateMesh() {
    // std::cout <<"Generating " << this << " with: " <<  myChunk <<std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    meshInterface = new btTriangleIndexVertexArray();
    btIndexedMesh tempMesh;
    meshInterface->addIndexedMesh(tempMesh);

    btIndexedMesh& mesh = meshInterface->getIndexedMeshArray()[0];
    std::vector<glm::vec4>* geometry = myChunk->getGeometryPointer();

    mesh.m_numTriangles = myChunk->getGeometrySize() / 3;
    mesh.m_triangleIndexBase = new unsigned char[sizeof(int) * myChunk->getGeometrySize()];
    mesh.m_indexType = PHY_INTEGER;
    mesh.m_triangleIndexStride = 3 * sizeof(int);

    mesh.m_numVertices = geometry->size();
    mesh.m_vertexBase = new unsigned char[4 * sizeof(float) * myChunk->getGeometrySize()];
    mesh.m_vertexStride = 4 * sizeof(float);

    memcpy((void*)mesh.m_triangleIndexBase, indices.data(),sizeof(int)*myChunk->getGeometrySize());

    memcpy((void*)mesh.m_vertexBase,geometry->data(),sizeof(glm::vec4)*myChunk->getGeometrySize());

    myShape = new btBvhTriangleMeshShape(meshInterface,true);

    btTransform trans;
    trans.setIdentity();
    
    myMotionState = new btDefaultMotionState(trans);
    
    body = new btRigidBody(0,myMotionState,myShape);
    
}

void ChunkMesh::addToWorld() {
    PhysicsWorld::addRigidBody(body);
}

void ChunkMesh::tryDelete() {
    MeshManager::manager->addTaskSingleThreaded(this);
}

void ChunkMesh::cleanUp() {
    
    delete body->getMotionState();
    delete body->getCollisionShape();
    delete body;
    delete meshInterface;
}