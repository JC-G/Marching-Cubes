#include "RoadAction.h"

#include "Editing.h"
#include "Preview.h"
#include "Window.h"
#include "RoadBrush.h"
#include "BezierHelperFunctions.h"

RoadAction::RoadAction(float r)
:radius(r)
{

}

void RoadAction::handleInput(glm::vec3 inPos) {
    Action::handleInput(inPos);
    if (Controller::keyPressed(Window::window,GLFW_KEY_X)) {
        finishSpline();
    }
}
void RoadAction::finishSpline() {
    std::vector<glm::vec3> controlPoints = BezierHelperFunctions::calculateControlPoints(points);
    if (controlPoints.size() == 0) {
        points.clear();
        return;
    }
    for (int i = 0; i < points.size() - 1; i++) {
        Brush* b = new RoadBrush(
            controlPoints[3 * i],
            controlPoints[3 * i + 1],
            controlPoints[3 * i + 2],
            controlPoints[3 * i + 3],
            radius
        );
        Editing::placeBrush(b,Window::mainOctree);
    }
    points.clear();
}

void RoadAction::onCancel() {
    points.clear();
}

std::string RoadAction::getDescription() {
    return "Road, radius " + std::to_string(radius) +", Points: " + std::to_string(points.size());
}

void RoadAction::increaseSize() {
    if (points.size() > 0) {
        return;
    }
    radius += 0.01;
}

void RoadAction::decreaseSize() {
    if (points.size() > 0) {
        return;
    }
    radius -= 0.01;
}

void RoadAction::onMouseDown(glm::vec3 inPos) {
    points.push_back(inPos);
}

void RoadAction::drawPreview() {
    if (points.size() == 0) {
        return;
    }
    std::vector<glm::vec3> tempPoints(points);
    tempPoints.push_back(Window::placePos);
    std::vector<glm::vec3> controlPoints = BezierHelperFunctions::calculateControlPoints(tempPoints);
    for (int i = 0; i < tempPoints.size()-1; i++) {
        Preview::drawBezierCurve(
            controlPoints[3 * i],
            controlPoints[3 * i + 1],
            controlPoints[3 * i + 2],
            controlPoints[3 * i + 3],
            16
        );
    }
}

std::string RoadAction::getDetails() {
    return "Brush Controls:\n"
           "([) Decrease Radius\n"
           "(]) Increase Radius\n"
           "(Click) Place Interpolation Point\n"
           "(X) Finish Spline";
}