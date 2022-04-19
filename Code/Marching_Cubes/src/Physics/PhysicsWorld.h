#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include <btBulletDynamicsCommon.h>
#include "PlayerController.h"
class PhysicsWorld {
    public:
        static bool init();
        static void setDebug(bool value);
        static void addRigidBody(btRigidBody* body);
        static void removeRigidBody(btRigidBody* body);
        static void debugDraw();
        static void step(float time);
        static void movePlayerFromControl(glm::vec3 movement);
        static void movePlayerToPosition(glm::vec3 position);
        static btRigidBody* getPlayerBody();


    private:
        PhysicsWorld(); //static class
        static btAlignedObjectArray<btCollisionShape*> collisionShapes;
        static btDiscreteDynamicsWorld* dynamicsWorld;
        static btIDebugDraw* debugDrawer;
        static PlayerController* player;
        static void tickCallback(btDynamicsWorld* world, btScalar timestep);


};

#endif