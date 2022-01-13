#include "CubicBezierAction.h"

CubicBezierAction::CubicBezierAction(float r)
:radius(r)
{

}

void CubicBezierAction::onMouseDown(glm::vec3 pos) {
    if (placingStep == 0) {
        A = pos;
        placingStep++;
    } else if (placingStep == 1) {
        B = pos;
        placingStep++;
    } else if (placingStep == 2) {
        C = pos;
        placingStep++;
    } else if (placingStep == 3) {
        D = pos;
        placingStep = 0;
        Brush* b = new CubicBezierBrush(A,B,C,D,radius);
        Editing::allBrushes.push_back(b);
        Editing::newBrushes.push_back(b);
    }
}

void CubicBezierAction::onCancel() {
    placingStep = 0;
}

std::string CubicBezierAction::getDescription() {
    return "Cubic Bezier Curve, Radius " + std::to_string(radius) + ", Placing Step " + std::to_string(placingStep);

}

void CubicBezierAction::increaseSize() {
    if (placingStep != 0) {
        return;
    }
    radius += 0.01;
}

void CubicBezierAction::decreaseSize() {
    if (placingStep != 0) {
        return;
    }
    radius -= 0.01;
}

void CubicBezierAction::drawPreview() {
    if (placingStep == 0) {
        return;
    }
    if (placingStep == 1) {
        Preview::drawPreviewLine(A,Window::placePos);
    } else if (placingStep == 2) {
        Preview::drawPreviewLine(A,B);
        Preview::drawPreviewLine(B,Window::placePos);
    } else if (placingStep == 3) {
        Preview::drawPreviewLine(A,B);
        Preview::drawPreviewLine(B,C);
        Preview::drawPreviewLine(C,Window::placePos);
    }
}