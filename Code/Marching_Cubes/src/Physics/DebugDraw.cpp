#include "DebugDraw.h"

#include "Preview.h"
#include "Text.h"
void DebugDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
    Preview::drawPreviewLine(glm::vec3(from.x(),from.y(),from.z()), glm::vec3(to.x(),to.y(),to.z()));
}

void DebugDraw::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) {
    //do nothing, for now
}

void DebugDraw::reportErrorWarning(const char* warningString) {
    std::cout << warningString << std::endl;
}

void DebugDraw::draw3dText(const btVector3& location,const char* textString) {
    Text::renderText(textString, location.x(), location.y(), 0.5, glm::vec3(0.0));
}

void DebugDraw::setDebugMode(int debugMode) {
    myDebugMode = debugMode;
}

int DebugDraw::getDebugMode() const {
    return myDebugMode;
}