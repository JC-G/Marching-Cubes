#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include <btBulletDynamicsCommon.h>

class PhysicsWorld {
    public:
        static bool init();
        static void setDebug(bool value);
        static void addRigidBody(btRigidBody* body);
        static void removeRigidBody(btRigidBody* body);
        static void debugDraw();
        static void step();

        //Testing purposes only...
        static void loadTestObject();
        static void drawTestObject();
        static void deleteTestObject();

        static btRigidBody* testBody;


    private:
        PhysicsWorld(); //static class
        static btAlignedObjectArray<btCollisionShape*> collisionShapes;
        static btDiscreteDynamicsWorld* dynamicsWorld;
        static btIDebugDraw* debugDrawer;


};

#endif