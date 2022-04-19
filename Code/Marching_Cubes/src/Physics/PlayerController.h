#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <btBulletDynamicsCommon.h>
#include "GL_headers.h"

class PlayerController {
    public:
        PlayerController();
        void moveFromVec3(glm::vec3 movementVector);
        btRigidBody* rigidBody;

    private:
        btCollisionShape* shape;

};


#endif