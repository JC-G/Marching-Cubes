#ifndef BEZIERHELPERFUNCTIONS_H
#define BEZIERHELPERFUNCTIONS_H
#include <vector>
#include "GL_headers.h"
class BezierHelperFunctions {
    public:
        static std::vector<glm::vec3> calculateControlPoints(std::vector<glm::vec3> interpolationPoints);
        static glm::vec3 cubicBezier(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, float t);
};
#endif