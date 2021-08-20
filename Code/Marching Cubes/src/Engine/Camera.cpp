#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
Camera::Camera()
{
    //ctor
}

Camera::~Camera()
{
    //dtor
}
glm::mat4 Camera::getViewMatrix()
{
    //Look in this direction
    glm::vec3 direction = glm::vec3(
        cos(rotation[1])*sin(rotation[0]),
        sin(rotation[1]),
        cos(rotation[1]) * cos(rotation[0])
    );
    return glm::lookAt(position, position+direction, glm::vec3(0,1,0));
}
