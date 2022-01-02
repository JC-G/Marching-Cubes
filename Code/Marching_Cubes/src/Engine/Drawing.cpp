#include "Drawing.h"

#include <glm/gtc/matrix_transform.hpp>
GLuint Drawing::chunkVAO = 0;
GLuint Drawing::lineVAO = 0;
GLuint Drawing::screenVAO = 0;
GLuint Drawing::textVAO = 0;
GLuint Drawing::screenBuffer = 0;
GLuint Drawing::textBuffer = 0;
Shader* Drawing::chunkShader;
Shader* Drawing::lineShader;
Shader* Drawing::screenShader;
Shader* Drawing::boxShader;
Shader* Drawing::textShader;
int Drawing::bufferWidth;
int Drawing::bufferHeight;
GLuint Drawing::frameBuffer = 0;
GLuint Drawing::depthBuffer = 0;
GLuint Drawing::frameBufferTexture = 0;
GLuint Drawing::crosshairTexture;
std::map<GLchar, Drawing::Character> Drawing::Characters;

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
    glGenVertexArrays(1, &textVAO);

    chunkShader = new Shader(Shader::ShaderFromFiles("Shaders/vert.glsl","Shaders/frag.glsl"));
    lineShader = new Shader(Shader::ShaderFromFiles("Shaders/line_vert.glsl","Shaders/line_frag.glsl"));
    screenShader = new Shader(Shader::ShaderFromFiles("Shaders/screen_vert.glsl","Shaders/screen_frag.glsl"));
    boxShader = new Shader(Shader::ShaderFromFiles("Shaders/box_vert.glsl","Shaders/box_frag.glsl"));
    textShader = new Shader(Shader::ShaderFromFiles("Shaders/text_vert.glsl","Shaders/text_frag.glsl"));


    //set uniforms that never change
    glm::mat4 PM = Window::getProjectionMatrix();

    glUseProgram(chunkShader->getID());
    glUniformMatrix4fv(chunkShader->getUniform("P"),1,GL_FALSE,&PM[0][0]);

    glUseProgram(lineShader->getID());
    glUniformMatrix4fv(lineShader->getUniform("P"),1,GL_FALSE,&PM[0][0]);

	glUseProgram(textShader->getID());
    glm::mat4 projection = glm::ortho(0.0f, (float)Config::get<int>("screen_width"), 0.0f, (float)Config::get<int>("screen_height"));
	
	
	glUniformMatrix4fv(textShader->getUniform("projection"),1,GL_FALSE,&projection[0][0]);
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

	glGenBuffers(1,&textBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,textBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    //Load font
    if (!initText()) {
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

bool Drawing::initText() {

    //TODO - the way this is included with CMAKE is not particularly nice...
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "Could not initialize freetype library" << std::endl;
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft,"fonts/arial.ttf", 0, &face)) {
        std::cout << "Could not load font" << std::endl;
        return false;
    }

	//https://learnopengl.com/code_viewer_gh.php?code=src/7.in_practice/2.text_rendering/text_rendering.cpp
	FT_Set_Pixel_Sizes(face,0,48);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	for (unsigned char c = 0; c < 128; c++) {
		int x = FT_Load_Char(face,c,FT_LOAD_RENDER);
		if (x) {
			std::cout << "Loading Glyph Failed: " << x << std::endl;
			continue;
		}
		// std::cout << c << " : " << face->glyph->bitmap.width << " , " << face->glyph->bitmap.rows << std::endl;
		GLuint texture;
		glGenTextures(1,&texture);
		glBindTexture(GL_TEXTURE_2D,texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


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
	drawGUIBox(glm::vec2(Window::width/2, Window::height/2)-glm::vec2(16),glm::vec2(32),crosshairTexture);
	RenderText(Editing::currentAction()->getDescription(), 25.0f, 25.0f, 0.5, glm::vec3(0.0));
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

void Drawing::RenderText(std::string text, float x, float y, float scale, glm::vec3 color) {
	glUseProgram(textShader->getID());
	glUniform3fv(textShader->getUniform("textColor"),1,&color[0]);
	glActiveTexture(GL_TEXTURE0);
	
	glBindVertexArray(textVAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,textBuffer);
    glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,4*sizeof(float),NULL);

	
    // iterate through all characters
    std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}