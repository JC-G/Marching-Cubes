#include "ChunkMesh.h"

#include "GL_headers.h"
#include "PhysicsWorld.h"

int ChunkMesh::maxIndex = 0;
std::vector<int> ChunkMesh::indices;

ChunkMesh::ChunkMesh(MarchingChunk* chunk) {
    chunk->mapGeometry();
    resizeIndices(chunk->getGeometrySize());

    //need to delete this
    meshInterface = new btTriangleIndexVertexArray();
    btIndexedMesh tempMesh;
    meshInterface->addIndexedMesh(tempMesh);

    btIndexedMesh& mesh = meshInterface->getIndexedMeshArray()[0];
    std::vector<glm::vec4>* geometry = chunk->getGeometryPointer();

    mesh.m_numTriangles = chunk->getGeometrySize() / 3;
    mesh.m_triangleIndexBase = new unsigned char[sizeof(int) * chunk->getGeometrySize()];
    mesh.m_indexType = PHY_INTEGER;
    mesh.m_triangleIndexStride = 3 * sizeof(int);

    mesh.m_numVertices = geometry->size();
    mesh.m_vertexBase = new unsigned char[4 * sizeof(float) * chunk->getGeometrySize()];
    mesh.m_vertexStride = 4 * sizeof(float);


    // part.m_vertexBase = chunk->getGeometryPointer();
    // part.m_vertexStride = 4 * sizeof(GLfloat);
    // part.m_numVertices = chunk->getGeometrySize();
    // part.m_vertexType = PHY_FLOAT;
    
    // part.m_triangleIndexBase = static_cast<const unsigned char*>((void*)indices.data());
    // part.m_triangleIndexStride = 3 * sizeof(int);
    // part.m_numTriangles = chunk->getGeometrySize() / 3;
    // part.m_indexType = PHY_INTEGER;

    memcpy((void*)mesh.m_triangleIndexBase, indices.data(),sizeof(int)*chunk->getGeometrySize());

    memcpy((void*)mesh.m_vertexBase,geometry->data(),sizeof(glm::vec4)*chunk->getGeometrySize());

    //need to delete this
    myShape = new btBvhTriangleMeshShape(meshInterface,true);

    btTransform trans;
    trans.setIdentity();
    
    myMotionState = new btDefaultMotionState(trans);

    body = new btRigidBody(0,myMotionState,myShape);
    PhysicsWorld::addRigidBody(body);

}

ChunkMesh::~ChunkMesh() {

//deletion order matters here?
//for some reason meshes are not getting deleted. Possibly the marchingchunk objects not getting deleted, not sure - the shared ptr might be doing something funny
    delete body->getMotionState();
    delete body->getCollisionShape();
    PhysicsWorld::removeRigidBody(body);
    delete body;
    delete meshInterface;

}

void ChunkMesh::resizeIndices(int maxVal) {
    for (int i = maxIndex; i < maxVal; i++) {
        indices.push_back(i);
    }
    maxIndex = maxVal;
}