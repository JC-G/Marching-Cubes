#include "BezierAction.h"

BezierAction::BezierAction(float r)
:radius(r)
{

}

void BezierAction::onMouseDown(glm::vec3 pos) {
    if (placingStep == 0) {
        A = pos;
        placingStep++;
    } else if (placingStep == 1) {
        B = pos;
        placingStep++;
    } else if (placingStep == 2) {
        C = pos;
        placingStep = 0;
        Brush* b = new BezierBrush(A,B,C,radius);
        Editing::allBrushes.push_back(b);
        Editing::newBrushes.push_back(b);
    }
}

void BezierAction::onCancel() {
    placingStep = 0;
}

std::string BezierAction::getDescription() {
    return "Bezier Curve, Radius " + std::to_string(radius) + ", Placing Step " + std::to_string(placingStep);
}

void BezierAction::increaseSize() {
    if (placingStep != 0) {
        return;
    }
    radius += 0.01;
}

void BezierAction::decreaseSize() {
    if (placingStep != 0) {
        return;
    }
    radius -= 0.01;
}

void BezierAction::drawPreview() {
    if (placingStep == 0) {
        return;
    }
    if (placingStep == 1) {
        Drawing::drawPreviewLine(A,Window::placePos);
    } else if (placingStep == 2) {
        Drawing::drawPreviewLine(A,B);
        Drawing::drawPreviewLine(B,Window::placePos);
    }
}

