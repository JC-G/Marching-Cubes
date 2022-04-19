#include "PlayerController.h"
#include "PhysicsWorld.h"
#include "Config.h"
PlayerController::PlayerController() {
    shape = new btCapsuleShape(0.3,1.2);
    float mass = 80.0;
    btTransform startPosition;
    startPosition.setIdentity();
    startPosition.setOrigin(Config::getBtVec3("camera_start_position"));
    btDefaultMotionState* motionState = new btDefaultMotionState(startPosition);
    btRigidBody::btRigidBodyConstructionInfo CI(mass,motionState,shape);

    rigidBody = new btRigidBody(CI);

    rigidBody->setDamping(0.2,0.2);
    rigidBody->setActivationState(DISABLE_DEACTIVATION);
    rigidBody->setAngularFactor(0);
    PhysicsWorld::addRigidBody(rigidBody);

    
}

void PlayerController::moveFromVec3(glm::vec3 movementVector) {
    rigidBody->applyCentralForce(btVector3(movementVector.x,movementVector.y,movementVector.z));
}

void PlayerController::moveToPosition(glm::vec3 position) {
    btTransform tr = rigidBody->getCenterOfMassTransform();
    tr.setOrigin(btVector3(position.x,position.y -Config::get<float>("player_height_offset"),position.z));
    rigidBody->setCenterOfMassTransform(tr);
}