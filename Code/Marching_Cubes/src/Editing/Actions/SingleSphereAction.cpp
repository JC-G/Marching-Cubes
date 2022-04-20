#include "SingleSphereAction.h"

SingleSphereAction::SingleSphereAction(float r) : SphereAction(r){};

void SingleSphereAction::onMouseDown(glm::vec3 pos) {
    SphereAction::onMouseHold(pos);
}

void SingleSphereAction::onMouseHold(glm::vec3 pos) {};

std::string SingleSphereAction::getDescription() {
    return "Single Sphere, Radius " + std::to_string(radius);

}

std::string SingleSphereAction::getDetails() {
    return "Brush Controls:\n"
           "([) Decrease Size\n"
           "(]) Increase Size\n"
           "(Click) Place Spheres";
}