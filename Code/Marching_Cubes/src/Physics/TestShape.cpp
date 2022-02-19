#include "TestShape.h"
#include "Preview.h"
#include "PhysicsWorld.h"

std::vector<TestShape*> TestShape::allShapes;

TestShape::TestShape(glm::vec3 pos) {
    btCollisionShape* testShape = new btSphereShape(btScalar(0.1));
    btTransform testTransform;
    testTransform.setIdentity();
    btScalar mass(1.f);

    btVector3 localInertia(0,0,0);
    testShape->calculateLocalInertia(mass,localInertia);

    testTransform.setOrigin(btVector3(pos.x,pos.y,pos.z));

    btDefaultMotionState* testMotionState = new btDefaultMotionState(testTransform);
    btRigidBody::btRigidBodyConstructionInfo testInfo(mass,testMotionState,testShape,localInertia);
    testBody = new btRigidBody(testInfo);

    PhysicsWorld::addRigidBody(testBody);
    allShapes.push_back(this);

}

void TestShape::draw() {
    btTransform trans;
    testBody->getMotionState()->getWorldTransform(trans);
    glm::vec3 drawPos = glm::vec3(
        trans.getOrigin().getX(),
        trans.getOrigin().getY(),
        trans.getOrigin().getZ()
    );
    Preview::drawPreviewSphere(glm::vec3(0.1),drawPos);
}

void TestShape::drawAll() {
    for (auto shape : allShapes) {
        shape->draw();
    }
}

glm::vec3 TestShape::getPosition() {
    btTransform trans;
    testBody->getMotionState()->getWorldTransform(trans);
    glm::vec3 pos = glm::vec3(
        trans.getOrigin().getX(),
        trans.getOrigin().getY(),
        trans.getOrigin().getZ()
    );
    return pos;
}

std::vector<glm::vec3> TestShape::getShapePositions() {
    std::vector<glm::vec3> poss;
    for (auto shape : allShapes) {
        poss.push_back(shape->getPosition());
    }
    return poss;
}