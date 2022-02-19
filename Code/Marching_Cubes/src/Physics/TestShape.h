#ifndef TESTSHAPE_H
#define TESTSHAPE_H

#include "GL_headers.h"
#include <btBulletDynamicsCommon.h>

class TestShape {
    public:
        TestShape(glm::vec3 pos);
        void draw();
        static void drawAll();
        glm::vec3 getPosition();
        static std::vector<glm::vec3> getShapePositions();
    private:
        btRigidBody* testBody;
        static std::vector<TestShape*> allShapes;
};


#endif