#include "ScaledTerrain.h"

ScaledTerrain::ScaledTerrain() {}

ScaledTerrain::~ScaledTerrain() {}

float ScaledTerrain::distance(glm::vec3 inPos) {return 0.0f;}

glm::vec3 ScaledTerrain::normal(glm::vec3 inPos){return glm::vec3(0,1,0);}

std::string ScaledTerrain::getShaderCode() {
    //Hills with a max height of 1000, grid scale 500
    return
    R"SHADER(
        float sdBox( vec3 p, vec3 b )
        {
            vec3 q = abs(p) - b;
            return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
        }

        vec3 sdBoxNormal(vec3 p, vec3 b) {
            //"easy" normals
            vec3 q = abs(p) - b;
            if (q.x > q.y && q.x > q.z) {
                return vec3(1,0,0) * sign(p);

            } else if (q.y > q.z) {
                return vec3(0,1,0) * sign(p);
            } else {
                return vec3(0,0,1) * sign(p);
            }
        }



        float distance(vec3 inPos) {
            float ground = inPos.y-biomeHeight(
                inPos.xz,
                3.0,
                4,
                2000.,
                2000.,
                0.4,
                2.0
            ).z;

            float box = sdBox(inPos-vec3(0.5,1,0.5),vec3(0.5,1,0.5));
            return min(box,ground);
        }

        vec3 normal(vec3 inPos) {
            vec3 n = biomeHeight(
                inPos.xz,
                3.0,
                4,
                2000.,
                2000.,
                0.4,
                2.0
            );
            return -normalize(
                vec3(n.x,-1,n.y)
            );

        }
    
    
    
    
    )SHADER";
}

