#include "PhysicsWorld.h"

#include <btBulletDynamicsCommon.h>

#include "DebugDraw.h"
#include <iostream>

btAlignedObjectArray<btCollisionShape*> PhysicsWorld::collisionShapes;
btDiscreteDynamicsWorld* PhysicsWorld::dynamicsWorld;
btIDebugDraw* PhysicsWorld::debugDrawer;

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
    setDebug(true);
    return true;
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

