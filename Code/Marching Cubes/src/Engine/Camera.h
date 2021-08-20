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

    protected:

    private:
};

#endif // CAMERA_H
