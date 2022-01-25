#ifndef PREVIEW_H
#define PREVIEW_H
#include "GL_headers.h"

#include "Shader.h"
class Preview {
    public:
        static bool initPreview();
        
        static void drawPreviewSphere(glm::vec3 radius, glm::vec3 position);
        static void drawPreviewCylinder(float radius, glm::vec3 pos1, glm::vec3 pos2,glm::vec4 color = glm::vec4(1.0,0.2,0.2,.3));
        static void drawPreviewLine(glm::vec3 pos1, glm::vec3 pos2);
        static void drawBezierCurve(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, int resolution);
    private:
        static GLuint textVAO;
        static Shader* previewShader;

        static GLuint previewVertexBuffer;
        static GLuint previewNormalBuffer;
        
	    static std::vector<glm::vec4> sphereVertexData;
	    static std::vector<glm::vec4> sphereNormalData;
        static std::vector<glm::vec4> cylinderVertexData;

        static bool parseOBJ(std::string path, std::vector<glm::vec4>& vertices);

};

#endif