#include "BezierHelperFunctions.h"
#include <iostream>
std::vector<glm::vec3> BezierHelperFunctions::calculateControlPoints(std::vector<glm::vec3> interpolationPoints) {
    std::vector<glm::vec3> controlPoints;
    int nCurves = interpolationPoints.size() - 1;
    if (nCurves < 2) {
        if (nCurves == 1) {
            //special case of 2 interpolation points only - generate a straight bezier curve
            controlPoints.push_back(interpolationPoints[0]);
            controlPoints.push_back(interpolationPoints[0]);
            controlPoints.push_back(interpolationPoints[1]);
            controlPoints.push_back(interpolationPoints[1]);
        }
        //otherwise, no control points - this means nothing will be generated
        
        return controlPoints;
    }
    
    //see https://towardsdatascience.com/b%C3%A9zier-interpolation-8033e9a262c2
    //and https://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
    //for details

    float c[nCurves-1]; //c' on wiki page, indexed from 0 to n-2 rather than 1 - n-1
    glm::vec3 d[nCurves]; //d' on wiki page, indexed from 0 to n-1 rather than 1 - n
    glm::vec3 A[nCurves]; //first non-interpolated control point
    glm::vec3 B[nCurves]; //second non-interpolated control point

    //Calculate c'
    c[0] = 0.5f; //c'[0] = c[0]/b[0] = 1/2
    for (int i = 1; i < nCurves - 1; i++) {
        c[i] = 1.0f/(4.0f-c[i-1]); //c'[i] = c[i]/(b[i]-a[i]c'[i-1])
    }

    //Calculate d'
    d[0] = 0.5f * (interpolationPoints[0] + 2.0f * interpolationPoints[1]); //d'[0] = d[0]/b[0]

    for (int i = 1; i < nCurves - 1; i++) {
        glm::vec3 k = 2.0f * (2.0f * interpolationPoints[i] + interpolationPoints[i+1]); //d[i]
        d[i] = (k-d[i-1])/(4.0f-c[i-1]); //d'[i] = (d[i]-a[i]d'[i-1])/(b[i]-a[i]c'[i-1])
    }

    glm::vec3 kn = 8.0f*interpolationPoints[nCurves-1] + interpolationPoints[nCurves];
    d[nCurves - 1] = (kn-2.0f*d[nCurves-2])/(7.0f-2.0f*c[nCurves-2]);

    //Calculate A
    A[nCurves - 1] = d[nCurves - 1];
    for (int i = nCurves - 2; i >= 0; i--) {
        A[i] = d[i]-c[i]*A[i+1];
    }

    //Calculate B
    for (int i = 0; i < nCurves - 1; i++) {
        B[i] = 2.0f * interpolationPoints[i+1] - A[i+1];
    }
    B[nCurves - 1] = 0.5f * (A[nCurves - 1] + interpolationPoints[nCurves]);

    for (int i = 0; i < nCurves; i++) {
        controlPoints.push_back(interpolationPoints[i]);
        controlPoints.push_back(A[i]);
        controlPoints.push_back(B[i]);
    }
    controlPoints.push_back(interpolationPoints[nCurves]);
    return controlPoints;
}

glm::vec3 BezierHelperFunctions::cubicBezier(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, float t) {
    return (1.0f - t) * (1.0f - t) * (1.0f - t) * A +
           3.0f * (1.0f - t) * (1.0f - t) * t * B +
           3.0f * (1.0f - t) * t * t * C +
           t * t * t * D;
}