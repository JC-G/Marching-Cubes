#ifndef TESTSHAPE_H
#define TESTSHAPE_H

#include "GL_headers.h"
#include <btBulletDynamicsCommon.h>
#include "BrushBoundingBox.h"

class TestShape {
    public:
        TestShape(glm::vec3 pos);
        void draw();
        static void drawAll();
        glm::vec3 getPosition();
        // static std::vector<glm::vec3> getShapePositions();
        BrushBoundingBox getBoundingBox();
        static std::vector<TestShape*> allShapes;
    private:
        btRigidBody* testBody;
};


#endif