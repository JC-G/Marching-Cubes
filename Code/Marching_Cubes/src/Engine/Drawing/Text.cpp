#include "Text.h"


GLuint Text::textVAO = 0;
Shader* Text::textShader;
GLuint Text::textBuffer = 0;
float Text::scaledHeight;

std::map<GLchar, Text::Character> Text::Characters;

bool Text::initText() { //Setup for text rendering
    glGenVertexArrays(1, &textVAO);
    textShader = new Shader(Shader::ShaderFromFiles("Shaders/text_vert.glsl","Shaders/text_frag.glsl"));

	glUseProgram(textShader->getID());

    glm::mat4 projection = glm::ortho(0.0f, (float)Config::get<int>("screen_width"), 0.0f, (float)Config::get<int>("screen_height"));
	glUniformMatrix4fv(textShader->getUniform("projection"),1,GL_FALSE,&projection[0][0]);

	glGenBuffers(1,&textBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,textBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

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

    //measured in 1/64 pixels
    scaledHeight = face->size->metrics.height/64.0;

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


    return true;
}

void Text::renderText(std::string text, float x, float y, float scale, glm::vec3 color) {
	glUseProgram(textShader->getID());
	glUniform3fv(textShader->getUniform("textColor"),1,&color[0]);
	glActiveTexture(GL_TEXTURE0);
	
	glBindVertexArray(textVAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,textBuffer);
    glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,4*sizeof(float),NULL);

	
    // iterate through all characters
    std::string::const_iterator c;
    float xOrig = x;
	for (c = text.begin(); c != text.end(); c++) 
    {
        
        Character ch = Characters[*c];

        if (*c=='\n') {
            y -= scale * scaledHeight;
            x = xOrig;
            continue;
        }

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