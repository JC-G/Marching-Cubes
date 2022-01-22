#include "RoadAction.h"

#include "Editing.h"
#include "Preview.h"
#include "Window.h"
#include "RoadBrush.h"

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
    if (points.size() < 3) {
        points.clear();
        return;
    }
    //Use a sequence of CubicBezierBrushes

    //see https://towardsdatascience.com/b%C3%A9zier-interpolation-8033e9a262c2
    //and https://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
    //for details

    float c[points.size()-1];
    glm::vec3 d[points.size()];

    c[0] = 0.5f;
    for (int i = 1; i < points.size() - 1; i++) {
        c[i] = 1.0f/(4.0f-c[i-1]);
    }

    d[0] = 0.5f * (points[0]+ 2.0f * points[1]);
    for (int i = 1; i < points.size() - 1; i++) {
        glm::vec3 k = 2.0f * (2.0f * points[i] + points[i+1]);
        d[i] = (k-d[i-1])/(4.0f-c[i-1]);
    }

    glm::vec3 kn = 8.0f*points[points.size()-2] + points[points.size()-1];
    d[points.size()-1] = (kn-2.0f*d[points.size()-2])/(7.0f-2.0f*c[points.size()-2]);

    glm::vec3 A[points.size() - 1];
    glm::vec3 B[points.size() - 1];
    A[points.size() - 1] = d[points.size() - 1];
    for (int i = points.size()-2; i >= 0; i--) {
        A[i] = d[i]-c[i]*A[i+1];
    }
    for (int i = 0; i < points.size() - 2; i++) {
        B[i] = 2.0f * points[i+1] - A[i+1];
    }
    B[points.size() - 2] = 0.5f * (A[points.size() - 2] + points[points.size() - 1]);
    for (int i = 0; i < points.size() - 1; i++) {
        Brush* b = new RoadBrush(points[i],A[i],B[i],points[i+1],radius);
        Editing::newBrushes.push_back(b);
        Editing::allBrushes.push_back(b);
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
    for (int i = 0; i < points.size()-1; i++) {
        Preview::drawPreviewLine(points[i],points[i+1]);
    }
    Preview::drawPreviewLine(points[points.size()-1], Window::placePos);
}