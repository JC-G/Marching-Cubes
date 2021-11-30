#include "Editing.h"

std::vector<Brush*> Editing::allBrushes;
std::vector<Brush*> Editing::newBrushes;
float Editing::cr;
glm::vec3 Editing::cp1;
glm::vec3 Editing::cp2;
bool Editing::placing = false;

void Editing::placeSphere(glm::vec3 pos, float r) {
    Brush* b = new EllipsoidBrush(pos,glm::vec3(r));
    allBrushes.push_back(b);
    newBrushes.push_back(b);
}

void Editing::digSphere(glm::vec3 pos, float r){
    
    Brush* b = new EllipsoidBrush(pos,glm::vec3(r),1);
    allBrushes.push_back(b);
    newBrushes.push_back(b);
}


void Editing::beginCylinder(glm::vec3 pos, float r) {
    if (placing) {
        return;
    }
    placing = true;
    cp1 = pos;
    cr = r;
}

void Editing::endCylinder(glm::vec3 pos) {
    if (!placing) {
        return;
    }
    cp2 = pos;

    Brush* b = new CylinderBrush(cp1,cp2,cr);
    allBrushes.push_back(b);
    newBrushes.push_back(b);

    placing = false;
}  

void Editing::sphereRing(glm::vec3 pos, float ringR, int ringN, float r) {

    for (int i = 0; i < ringN; i++) {
        float theta = float(i)*glm::two_pi<float>()/float(ringN);
        glm::vec3 posi = pos + ringR * glm::vec3(cos(theta),0,sin(theta));
        Brush* b = new EllipsoidBrush(posi,glm::vec3(r));
        allBrushes.push_back(b);
        newBrushes.push_back(b);
    }
}

glm::vec3 Editing::rayCast(glm::vec3 pos, glm::vec3 direction, Octree* O) {
    if (Config::get<std::string>("raycast_mode") == "mesh") {
        float intersectionPoint = O->getIntersectionPoint(pos,direction);
        if (intersectionPoint == std::numeric_limits<float>::max()) {
            return pos; //TODO - temporary fix so that we don't get weird behaviour trying to place a shape infinitely far away
        }
        glm::vec3 p = pos + direction * intersectionPoint;
        return p;
    }

    
// //At the time of the first marching chunk creation, compile the raycasting shader for later use
//     if (!raycastShader) {
//         raycastShader = &(Shader::ComputeShaderFromVector(std::vector<std::string>{
//             Shader::ReadShaderFile("Shaders/Compute/shadertop.glsl"),
//             Generator->densityFunction->getShaderCode(),
//             Shader::ReadShaderFile("Shaders/Compute/brush_functions.glsl"),
//             Shader::ReadShaderFile("Shaders/Compute/terrain_modification.glsl"),
            
//         }
//         ));
//     }
}


