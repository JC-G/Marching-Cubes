#include "PhysicsWorld.h"

#include <btBulletDynamicsCommon.h>

#include "DebugDraw.h"
#include <iostream>

btAlignedObjectArray<btCollisionShape*> PhysicsWorld::collisionShapes;
btDiscreteDynamicsWorld* PhysicsWorld::dynamicsWorld;
btIDebugDraw* PhysicsWorld::debugDrawer;

//test only
btRigidBody* PhysicsWorld::testBody;

bool PhysicsWorld::init() {
    //setup the bullet physics world

    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    //set the gravity
    dynamicsWorld->setGravity(btVector3(0, -10, 0));
    return true;
    //setDebug(true);
}

void PhysicsWorld::setDebug(bool value) {
    if (!debugDrawer) {
        debugDrawer = new DebugDraw();
        dynamicsWorld->setDebugDrawer(debugDrawer);
    }
    if (value) {
        dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    }
}

void PhysicsWorld::addRigidBody(btRigidBody* body) {
    dynamicsWorld->addRigidBody(body);
}
void PhysicsWorld::removeRigidBody(btRigidBody* body) {
    dynamicsWorld->removeCollisionObject(body);
}

void PhysicsWorld::debugDraw() {
    dynamicsWorld->debugDrawWorld();
}

void PhysicsWorld::step() {
    dynamicsWorld->stepSimulation(1./600.); //maybe bad to do as fast as possible, we will see...
}

void PhysicsWorld::loadTestObject() {
    btCollisionShape* testShape = new btSphereShape(btScalar(0.1));
    btTransform testTransform;
    testTransform.setIdentity();
    btScalar mass(1.f);

    btVector3 localInertia(0,0,0);
    testShape->calculateLocalInertia(mass,localInertia);

    testTransform.setOrigin(btVector3(0,10,0));

    btDefaultMotionState* testMotionState = new btDefaultMotionState(testTransform);
    btRigidBody::btRigidBodyConstructionInfo testInfo(mass,testMotionState,testShape,localInertia);
    testBody = new btRigidBody(testInfo);

    PhysicsWorld::addRigidBody(testBody);
    
}

#include "Preview.h"

void PhysicsWorld::drawTestObject() {
    btTransform trans;
    testBody->getMotionState()->getWorldTransform(trans);
    glm::vec3 drawPos = glm::vec3(
        trans.getOrigin().getX(),
        trans.getOrigin().getY(),
        trans.getOrigin().getZ()
    );
    Preview::drawPreviewSphere(glm::vec3(0.1),drawPos);
}


void PhysicsWorld::deleteTestObject() {
    //nah lol
}