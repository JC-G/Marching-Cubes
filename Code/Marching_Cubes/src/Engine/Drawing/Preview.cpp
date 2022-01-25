#include "Preview.h"

#include "Window.h"
#include "BezierHelperFunctions.h"

GLuint Preview::textVAO;
GLuint Preview::previewVertexBuffer;
GLuint Preview::previewNormalBuffer;

std::vector<glm::vec4> Preview::sphereNormalData;
std::vector<glm::vec4> Preview::sphereVertexData;
std::vector<glm::vec4> Preview::cylinderVertexData;

Shader* Preview::previewShader;

bool Preview::initPreview() {

    glGenVertexArrays(1, &textVAO);
	previewShader = new Shader(Shader::ShaderFromFiles("Shaders/preview_vert.glsl", "Shaders/preview_frag.glsl"));

    glm::mat4 PM = Window::getProjectionMatrix();
	glUseProgram(previewShader->getID());
    glUniformMatrix4fv(previewShader->getUniform("P"),1,GL_FALSE,&PM[0][0]);

	glGenBuffers(1,&previewVertexBuffer);
	//glGenBuffers(1,&previewNormalBuffer);
	parseOBJ("Models/sphere.obj",sphereVertexData);
	parseOBJ("Models/cylinder.obj", cylinderVertexData);
	return true;
	
}

bool Preview::parseOBJ(std::string path, std::vector<glm::vec4>& vertices) {
//based on example from https://github.com/tinyobjloader/tinyobjloader
	tinyobj::ObjReader reader;
	if (!reader.ParseFromFile(path)) {
		if (!reader.Error().empty()) {
			std::cout << "OBJ file error: "<< reader.Error() << std::endl;
		} else {
			std::cout << "Unknown OBJ file error" << std::endl;
		}
	return false;
	}
	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();

	// std::vector<float> sphereVertexData;
	// std::vector<float> sphereNormalData;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
	// Loop over faces(polygon)
	size_t index_offset = 0;
	for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
		size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

		// Loop over vertices in the face.
		for (size_t v = 0; v < fv; v++) {
			// access to vertex
			tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
			tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
			tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
			tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
			vertices.push_back(glm::vec4(vx,vy,vz,1));

			// Check if `normal_index` is zero or positive. negative = no normal data
			if (idx.normal_index >= 0) {
				tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
				tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
				tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
				//sphereNormalData.push_back(glm::vec4(nx,ny,nz,0));
			}

			//there are no texture coordinates...

			// Check if `texcoord_index` is zero or positive. negative = no texcoord data
			// if (idx.texcoord_index >= 0) {
			// 	tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
			// 	tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
			// }

			}
			index_offset += fv;

		}
	}
	return true;
}

void Preview::drawPreviewSphere(glm::vec3 radius, glm::vec3 position) {
	glm::mat4 transform = glm::translate(position) * glm::scale(radius);

	glUseProgram(previewShader->getID());
	glUniformMatrix4fv(previewShader->getUniform("transform"),1,GL_FALSE,&transform[0][0]);
    glm::mat4 VM = Window::activeCamera->getViewMatrix();
	glUniformMatrix4fv(previewShader->getUniform("V"),1,GL_FALSE,&VM[0][0]);
	glm::vec4 color(1.0,0.2,0.2,.3);
	glUniform4fv(previewShader->getUniform("color"),1,&color[0]);

	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER,previewVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,sphereVertexData.size()*sizeof(glm::vec4),sphereVertexData.data(),GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glDrawArrays(GL_TRIANGLES,0,sphereVertexData.size());	

}

void Preview::drawPreviewCylinder(float radius, glm::vec3 pos1, glm::vec3 pos2,glm::vec4 color) {
	glm::mat4 transform = glm::translate(pos1) * glm::orientation(glm::normalize(pos2-pos1), glm::vec3(0,1,0)) * glm::scale(glm::vec3(radius,length(pos2-pos1),radius));

	glUseProgram(previewShader->getID());
	glUniformMatrix4fv(previewShader->getUniform("transform"),1,GL_FALSE,&transform[0][0]);
    glm::mat4 VM = Window::activeCamera->getViewMatrix();
	glUniformMatrix4fv(previewShader->getUniform("V"),1,GL_FALSE,&VM[0][0]);
	glUniform4fv(previewShader->getUniform("color"),1,&color[0]);

	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER,previewVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,cylinderVertexData.size()*sizeof(glm::vec4),cylinderVertexData.data(),GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glDrawArrays(GL_TRIANGLES,0,cylinderVertexData.size());
}

void Preview::drawPreviewLine(glm::vec3 pos1, glm::vec3 pos2) {
	//a cylinder is better than a line, which is 1 pixel wide...
	drawPreviewCylinder(0.01,pos1,pos2,glm::vec4(1,0,0,1));
}

void Preview::drawBezierCurve(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, int resolution) {
	for (int i = 0; i < resolution; i++) {
		float rf = float(resolution);
		float t = i / rf;
		float t1 = (i+1.0f) / rf;
		drawPreviewLine(
			BezierHelperFunctions::cubicBezier(A,B,C,D,t),
			BezierHelperFunctions::cubicBezier(A,B,C,D,t1)
		);
	}
}