#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>


Camera::Camera()
{
    position = glm::vec3(0.0,0.0,-5.0);
    rotation = glm::vec3(0.0);
    up = glm::vec3(0.0,1.0,0.0);
    speed = 0.1;
}

Camera::~Camera()
{
    //dtor
}
glm::mat4 Camera::getViewMatrix()
{
    //Look in this direction
    glm::vec3 direction = getDirection();
    return glm::lookAt(position, position+direction, glm::vec3(0,1,0));
}
void Camera::rotateFromMouse(double dX, double dY)
{
    rotation[0] += dX*0.01;
    rotation[1] += dY*0.01;

    //do not flip upside down
    rotation[1] = glm::clamp(rotation[1],(float)-1.57,(float)1.57);
}



void Camera::moveFromVec3(glm::vec3 movement) {
    glm::vec3 hDirection = glm::vec3(sin(rotation[0]),0,cos(rotation[0]));
    glm::vec3 rightDirection = glm::vec3(-cos(rotation[0]),0,sin(rotation[0]));

    position += speed * (hDirection * movement.x + up * movement.y + rightDirection * movement.z);

}

glm::vec3 Camera::getDirection() {
    return glm::vec3(
        cos(rotation[1])*sin(rotation[0]),
        sin(rotation[1]),
        cos(rotation[1]) * cos(rotation[0])
    );
}
