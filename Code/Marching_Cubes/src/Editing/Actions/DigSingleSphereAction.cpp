#include "DigSingleSphereAction.h"

DigSingleSphereAction::DigSingleSphereAction(float r) : DigSphereAction(r){};

void DigSingleSphereAction::onMouseDown(glm::vec3 pos) {
    DigSphereAction::onMouseHold(pos);
}

void DigSingleSphereAction::onMouseHold(glm::vec3 pos) {};

std::string DigSingleSphereAction::getDescription() {
    return "Dig Single Sphere, Radius " + std::to_string(radius);

}

std::string DigSingleSphereAction::getDetails() {
    return "Brush Controls:\n"
           "([) Decrease Size\n"
           "(]) Increase Size\n"
           "(Click) Dig Sphere";
}