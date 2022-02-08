#ifndef TESTSHAPE_H
#define TESTSHAPE_H

#include "GL_headers.h"
#include <btBulletDynamicsCommon.h>

class TestShape {
    public:
        TestShape(glm::vec3 pos);
        void draw();
        static void drawAll();
    private:
        btRigidBody* testBody;
        static std::vector<TestShape*> allShapes;
};


#endif