#ifndef CAMERA_H
#define CAMERA_H

#include "GL_headers.h"
class Camera
{
    public:
        glm::vec3 position;
        glm::vec3 rotation;
        Camera();
        virtual ~Camera();
        glm::mat4 getViewMatrix();
        glm::vec3 getDirection();

        void rotateFromMouse(double dX, double dY, double dt);
        void moveFromVec3(glm::vec3 movement, double dt);
    protected:

    private:
        glm::vec3 up;
        float speed;
};

#endif // CAMERA_H
