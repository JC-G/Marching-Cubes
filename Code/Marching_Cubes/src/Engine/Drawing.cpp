#include "Drawing.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
GLuint Drawing::chunkVAO = 0;
GLuint Drawing::lineVAO = 0;
GLuint Drawing::screenVAO = 0;
GLuint Drawing::screenBuffer = 0;
Shader* Drawing::chunkShader;
Shader* Drawing::lineShader;
Shader* Drawing::screenShader;
Shader* Drawing::boxShader;
Shader* Drawing::spherePreviewShader;
int Drawing::bufferWidth;
int Drawing::bufferHeight;
GLuint Drawing::frameBuffer = 0;
GLuint Drawing::depthBuffer = 0;
GLuint Drawing::frameBufferTexture = 0;
GLuint Drawing::crosshairTexture;
std::vector<glm::vec4> Drawing::sphereNormalData;
std::vector<glm::vec4> Drawing::sphereVertexData;
std::vector<glm::vec4> Drawing::cylinderVertexData;
GLuint Drawing::previewVertexBuffer;
GLuint Drawing::previewNormalBuffer;

const GLfloat Drawing::screenQuadData[] = { 
	-1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
};


bool Drawing::init() {
    
    // make and bind the VAO
    glGenVertexArrays(1, &chunkVAO);
    glGenVertexArrays(1, &lineVAO);
    glGenVertexArrays(1, &screenVAO);

    chunkShader = new Shader(Shader::ShaderFromFiles("Shaders/vert.glsl","Shaders/frag.glsl"));
    lineShader = new Shader(Shader::ShaderFromFiles("Shaders/line_vert.glsl","Shaders/line_frag.glsl"));
    screenShader = new Shader(Shader::ShaderFromFiles("Shaders/screen_vert.glsl","Shaders/screen_frag.glsl"));
    boxShader = new Shader(Shader::ShaderFromFiles("Shaders/box_vert.glsl","Shaders/box_frag.glsl"));
	spherePreviewShader = new Shader(Shader::ShaderFromFiles("Shaders/preview_vert.glsl", "Shaders/preview_frag.glsl"));


    //set uniforms that never change
    glm::mat4 PM = Window::getProjectionMatrix();

    glUseProgram(chunkShader->getID());
    glUniformMatrix4fv(chunkShader->getUniform("P"),1,GL_FALSE,&PM[0][0]);

    glUseProgram(lineShader->getID());
    glUniformMatrix4fv(lineShader->getUniform("P"),1,GL_FALSE,&PM[0][0]);

	glUseProgram(spherePreviewShader->getID());
    glUniformMatrix4fv(spherePreviewShader->getUniform("P"),1,GL_FALSE,&PM[0][0]);


    //create framebuffer
    glfwGetFramebufferSize(Window::window,&bufferWidth,&bufferHeight);
    glGenFramebuffers(1,&frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);

    //create blank texture
    glGenTextures(1,&frameBufferTexture);
    glBindTexture(GL_TEXTURE_2D,frameBufferTexture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,bufferWidth,bufferHeight,0,GL_RGB,GL_UNSIGNED_BYTE,0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //create and attach depth buffer
    glGenRenderbuffers(1,&depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,bufferWidth,bufferHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthBuffer);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,frameBufferTexture,0);

    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //glBindVertexArray(screenVAO);

    glGenBuffers(1,&screenBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,screenBuffer);
    glBufferData(GL_ARRAY_BUFFER,sizeof(screenQuadData),screenQuadData,GL_STATIC_DRAW);

    //Load font
    if (!Text::initText()) {
        return false;
    }

	if (!initOBJ()) {
		return false;
	}

    crosshairTexture = loadTexture("Textures/crosshair.dds");

    return true;
}

bool Drawing::drawFrame() {

    glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);
    glViewport(0,0,bufferWidth,bufferHeight);
    glClearColor(0,148./255,1,1);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawChunks();

    drawGUI();

    drawToScreen();

    return true;
}

bool Drawing::drawChunks() {

    
    glUseProgram(chunkShader->getID());

    //set the view matrix accordingly
    glm::mat4 VM = Window::activeCamera->getViewMatrix();
    glUniformMatrix4fv(chunkShader->getUniform("V"),1,GL_FALSE,&VM[0][0]);
    glUniform3fv(chunkShader->getUniform("cameraPosition"),1,&(Window::activeCamera->position)[0]);

    for (auto C : MarchingChunk::loadedChunks) {
        //for test chunks only, octree is used to store actual chunks
        C->draw(Drawing::chunkVAO);
    }
    if (Config::get<bool>("load_octree")) {
        Window::mainOctree->draw(Drawing::chunkVAO);
    }

    
    if (Config::get<bool>("draw_chunk_boundaries")) {
        glUseProgram(lineShader->getID());
        glUniformMatrix4fv(lineShader->getUniform("V"),1,GL_FALSE,&VM[0][0]);
        for (auto C : MarchingChunk::loadedChunks) {
            C->drawBoundary(Drawing::lineVAO);
        }
        if (Config::get<bool>("load_octree")) {
            Window::mainOctree->drawBoundary(Drawing::lineVAO);
        }
    }


    return true;

}

bool Drawing::drawToScreen() {
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,bufferWidth,bufferHeight);
    
    glBindVertexArray(screenVAO);

    glClearColor(1,148./255,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(screenShader->getID());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,frameBufferTexture);
    glUniform1i(screenShader->getUniform("renderedTexture"),0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,screenBuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
    
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDrawArrays(GL_TRIANGLES,0,6);

    glfwSwapBuffers(Window::window);
    return true;
}



GLuint Drawing::loadTexture(char const* Filename) { //https://github.com/g-truc/gli

	gli::texture Texture = gli::load(Filename);
	if(Texture.empty())
		return 0;

	gli::gl GL(gli::gl::PROFILE_GL33);
	gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
	GLenum Target = GL.translate(Texture.target());

	GLuint TextureName = 0;
	glGenTextures(1, &TextureName);
	glBindTexture(Target, TextureName);
	glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);

	glm::tvec3<GLsizei> const Extent(Texture.extent());
	GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

	switch(Texture.target())
	{
	case gli::TARGET_1D:
		glTexStorage1D(
			Target, static_cast<GLint>(Texture.levels()), Format.Internal, Extent.x);
		break;
	case gli::TARGET_1D_ARRAY:
	case gli::TARGET_2D:
	case gli::TARGET_CUBE:
		glTexStorage2D(
			Target, static_cast<GLint>(Texture.levels()), Format.Internal,
			Extent.x, Texture.target() == gli::TARGET_2D ? Extent.y : FaceTotal);
		break;
	case gli::TARGET_2D_ARRAY:
	case gli::TARGET_3D:
	case gli::TARGET_CUBE_ARRAY:
		glTexStorage3D(
			Target, static_cast<GLint>(Texture.levels()), Format.Internal,
			Extent.x, Extent.y,
			Texture.target() == gli::TARGET_3D ? Extent.z : FaceTotal);
		break;
	default:
		assert(0);
		break;
	}

	for(std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
	for(std::size_t Face = 0; Face < Texture.faces(); ++Face)
	for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
	{
		GLsizei const LayerGL = static_cast<GLsizei>(Layer);
		glm::tvec3<GLsizei> Extent(Texture.extent(Level));
		Target = gli::is_target_cube(Texture.target())
			? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
			: Target;

		switch(Texture.target())
		{
		case gli::TARGET_1D:
			if(gli::is_compressed(Texture.format()))
				glCompressedTexSubImage1D(
					Target, static_cast<GLint>(Level), 0, Extent.x,
					Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
					Texture.data(Layer, Face, Level));
			else
				glTexSubImage1D(
					Target, static_cast<GLint>(Level), 0, Extent.x,
					Format.External, Format.Type,
					Texture.data(Layer, Face, Level));
			break;
		case gli::TARGET_1D_ARRAY:
		case gli::TARGET_2D:
		case gli::TARGET_CUBE:
			if(gli::is_compressed(Texture.format()))
				glCompressedTexSubImage2D(
					Target, static_cast<GLint>(Level),
					0, 0,
					Extent.x,
					Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
					Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
					Texture.data(Layer, Face, Level));
			else
				glTexSubImage2D(
					Target, static_cast<GLint>(Level),
					0, 0,
					Extent.x,
					Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
					Format.External, Format.Type,
					Texture.data(Layer, Face, Level));
			break;
		case gli::TARGET_2D_ARRAY:
		case gli::TARGET_3D:
		case gli::TARGET_CUBE_ARRAY:
			if(gli::is_compressed(Texture.format()))
				glCompressedTexSubImage3D(
					Target, static_cast<GLint>(Level),
					0, 0, 0,
					Extent.x, Extent.y,
					Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
					Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
					Texture.data(Layer, Face, Level));
			else
				glTexSubImage3D(
					Target, static_cast<GLint>(Level),
					0, 0, 0,
					Extent.x, Extent.y,
					Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
					Format.External, Format.Type,
					Texture.data(Layer, Face, Level));
			break;
		default: assert(0); break;
		}
	}
	return TextureName;
}

bool Drawing::drawGUI() {
	//this must be drawn before the actual GUI, because of transparency...
	Editing::currentAction()->drawPreview();

	//now draw the actual GUI
	drawGUIBox(glm::vec2(Window::width/2, Window::height/2)-glm::vec2(16),glm::vec2(32),crosshairTexture);
	Text::renderText(Editing::currentAction()->getDescription(), 25.0f, 25.0f, 0.5, glm::vec3(0.0));
    return true;
}

void Drawing::drawGUIBox(glm::vec2 position, glm::vec2 size, GLuint texture){
	glUseProgram(boxShader->getID());
	glUniform2f(boxShader->getUniform("screenSize"),Window::width,Window::height);
	glUniform2fv(boxShader->getUniform("position"),1,&position[0]);
	glUniform2fv(boxShader->getUniform("size"),1,&size[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,crosshairTexture);
	glUniform1i(boxShader->getUniform("renderedTexture"),1);

	//reuse screen vao because it is essentially the same
	glBindVertexArray(screenVAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,screenBuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDrawArrays(GL_TRIANGLES,0,6);
	


}


bool Drawing::parseOBJ(std::string path, std::vector<glm::vec4>& vertices) {
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


bool Drawing::initOBJ() {

	glGenBuffers(1,&previewVertexBuffer);
	// // glGenBuffers(1,&previewNormalBuffer);
	parseOBJ("Models/sphere.obj",sphereVertexData);
	parseOBJ("Models/cylinder.obj", cylinderVertexData);
	return true;
	
}

void Drawing::drawPreviewSphere(glm::vec3 radius, glm::vec3 position) {
	glm::mat4 transform = glm::translate(position) * glm::scale(radius);

	glUseProgram(spherePreviewShader->getID());
	glUniformMatrix4fv(spherePreviewShader->getUniform("transform"),1,GL_FALSE,&transform[0][0]);
    glm::mat4 VM = Window::activeCamera->getViewMatrix();
	glUniformMatrix4fv(spherePreviewShader->getUniform("V"),1,GL_FALSE,&VM[0][0]);
	glm::vec4 color(1.0,0.2,0.2,.3);
	glUniform4fv(spherePreviewShader->getUniform("color"),1,&color[0]);

	glBindVertexArray(chunkVAO);
	glBindBuffer(GL_ARRAY_BUFFER,previewVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,sphereVertexData.size()*sizeof(glm::vec4),sphereVertexData.data(),GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glDrawArrays(GL_TRIANGLES,0,sphereVertexData.size());	

}

void Drawing::drawPreviewCylinder(float radius, glm::vec3 pos1, glm::vec3 pos2,glm::vec4 color) {
	glm::mat4 transform = glm::translate(pos1) * glm::orientation(glm::normalize(pos2-pos1), glm::vec3(0,1,0)) * glm::scale(glm::vec3(radius,length(pos2-pos1),radius));

	glUseProgram(spherePreviewShader->getID());
	glUniformMatrix4fv(spherePreviewShader->getUniform("transform"),1,GL_FALSE,&transform[0][0]);
    glm::mat4 VM = Window::activeCamera->getViewMatrix();
	glUniformMatrix4fv(spherePreviewShader->getUniform("V"),1,GL_FALSE,&VM[0][0]);
	glUniform4fv(spherePreviewShader->getUniform("color"),1,&color[0]);

	glBindVertexArray(chunkVAO);
	glBindBuffer(GL_ARRAY_BUFFER,previewVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,cylinderVertexData.size()*sizeof(glm::vec4),cylinderVertexData.data(),GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glDrawArrays(GL_TRIANGLES,0,cylinderVertexData.size());
}

void Drawing::drawPreviewLine(glm::vec3 pos1, glm::vec3 pos2) {
	//a cylinder is better than a line, which is 1 pixel wide...
	drawPreviewCylinder(0.01,pos1,pos2,glm::vec4(1,0,0,1));
}