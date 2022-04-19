#include "Camera.h"

#include "Config.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

Camera::Camera()
{
    position = Config::getVec3("camera_start_position");
    rotation = glm::vec3(0.0);
    up = glm::vec3(0.0,1.0,0.0);
    
}

Camera::~Camera()
{
    //dtor
}
glm::mat4 Camera::getViewMatrix()
{
    //Look in this direction
    glm::vec3 direction = getDirection();
    return glm::lookAt(position, position+direction, up);
}
void Camera::rotateFromMouse(double dX, double dY, double dt)
{
    rotation[0] += dX*dt;
    rotation[1] += dY*dt;

    //do not flip upside down
    rotation[1] = glm::clamp(rotation[1],(float)-1.57,(float)1.57);
}



void Camera::moveFromVec3(glm::vec3 movement, double dt) {
    position += (float)dt * getMovementVector(movement);
}

glm::vec3 Camera::getDirection() {
    return glm::vec3(
        cos(rotation[1])*sin(rotation[0]),
        sin(rotation[1]),
        cos(rotation[1]) * cos(rotation[0])
    );
}

glm::vec3 Camera::getMovementVector(glm::vec3 movement) {
    if (Config::get<bool>("player_physics")) {
        speed = Config::get<float>("physics_move_speed");
    } else {
        speed = Config::get<float>("camera_move_speed");
    }
    glm::vec3 hDirection = glm::vec3(sin(rotation[0]),0,cos(rotation[0]));
    glm::vec3 rightDirection = glm::vec3(-cos(rotation[0]),0,sin(rotation[0]));

    return speed * (hDirection * movement.x + up * Config::get<float>("jump_speed") * movement.y + rightDirection * movement.z);

}

void Camera::moveToShape(btRigidBody* body) {
    btVector3 newPos = body->getCenterOfMassPosition();
    position = glm::vec3(newPos.x(),newPos.y() + Config::get<float>("player_height_offset"),newPos.z());
}