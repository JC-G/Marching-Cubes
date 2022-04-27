#include "InterpolationSplineAction.h"

#include "Editing.h"
#include "Preview.h"
#include "Window.h"
#include "CubicBezierBrush.h"
#include "BezierHelperFunctions.h"

InterpolationSplineAction::InterpolationSplineAction(float r)
:radius(r)
{

}

void InterpolationSplineAction::handleInput(glm::vec3 inPos) {
    Action::handleInput(inPos);
    if (Controller::keyPressed(Window::window,GLFW_KEY_X)) {
        finishSpline();
    }
    if (Controller::keyPressed(Window::window,GLFW_KEY_C)) {
        mousePreview = !mousePreview;
    }
}
void InterpolationSplineAction::finishSpline() {
    //Use a sequence of CubicBezierBrushes
    std::vector<glm::vec3> controlPoints = BezierHelperFunctions::calculateControlPoints(points);
    if (controlPoints.size() == 0) {
        points.clear();
        return;
    }
    for (int i = 0; i < points.size() - 1; i++) {
        Brush* b = new CubicBezierBrush(
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

void InterpolationSplineAction::onCancel() {
    points.clear();
}

std::string InterpolationSplineAction::getDescription() {
    return "Interpolation Spline, radius " + std::to_string(radius) +", Points: " + std::to_string(points.size());
}

void InterpolationSplineAction::increaseSize() {
    if (points.size() > 0) {
        return;
    }
    radius += 0.01;
}

void InterpolationSplineAction::decreaseSize() {
    if (points.size() > 0) {
        return;
    }
    radius -= 0.01;
}

void InterpolationSplineAction::onMouseDown(glm::vec3 inPos) {
    points.push_back(inPos);
}

void InterpolationSplineAction::drawPreview() {
    if (points.size() == 0) {
        return;
    }
    std::vector<glm::vec3> tempPoints(points);
    if (mousePreview) {
        tempPoints.push_back(Window::placePos);
    }
    std::vector<glm::vec3> controlPoints = BezierHelperFunctions::calculateControlPoints(tempPoints);
    for (int i = 0; i < tempPoints.size()-1; i++) {
        Preview::drawBezierCurve(
            radius,
            controlPoints[3 * i],
            controlPoints[3 * i + 1],
            controlPoints[3 * i + 2],
            controlPoints[3 * i + 3],
            16
        );
    }

    // if (points.size() == 0) {
    //     return;
    // }
    // for (int i = 0; i < points.size()-1; i++) {
    //     Preview::drawPreviewLine(points[i],points[i+1]);
    // }
    // Preview::drawPreviewLine(points[points.size()-1], Window::placePos);
}

std::string InterpolationSplineAction::getDetails() {
    return "Brush Controls:\n"
           "([) Decrease Radius\n"
           "(]) Increase Radius\n"
           "(Click) Place Interpolation Point\n"
           "(X) Finish Spline\n"
           "(C) Toggle Mouse Preview";
}