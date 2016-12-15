#include "TextRenderer.h"

/* Loads a given font with the specified size */
TextRenderer::TextRenderer(const char* font, int size, float screenWidth, float screenHeight) {
	// Calculate projection matrix to be using during rendering
	mProjectionMatrix = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);

	// FreeType
	FT_Library ft;

	// All functions return a value different than 0 whenever an error occurred
	// Initialize FreeType library
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, font, 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}

	// Set size to load glyphs as
	// Setting the width to 0 lets the face dynamically calculate the width based on the given height
	FT_Set_Pixel_Sizes(face, 0, size);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < ASCII_COUNT; ++c) {
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph of character '" << c << "'" << std::endl;
			continue;
		}

		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
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

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Now store character for later use
		this->mCharacters[c].TextureID = texture;
		this->mCharacters[c].Width = face->glyph->bitmap.width;
		this->mCharacters[c].Height = face->glyph->bitmap.rows;
		this->mCharacters[c].BearingX = face->glyph->bitmap_left;
		this->mCharacters[c].BearingY = face->glyph->bitmap_top;
		this->mCharacters[c].Advance = face->glyph->advance.x >> 6;	// Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}

	// Unbind texture target
	glBindTexture(GL_TEXTURE_2D, 0);

	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

	// Unbind VAO/VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Disable attributes
	glDisableVertexAttribArray(0);
}

/* Destructs the loaded font */
TextRenderer::~TextRenderer() {
	// Release data from the memory
	glDeleteBuffers(1, &this->VBO);
	glDeleteVertexArrays(1, &this->VAO);

	// Release textures
	for (GLubyte c = 0; c < ASCII_COUNT; ++c) {
		glDeleteTextures(1, &this->mCharacters[c].TextureID);
	}
}

/* Draws the given text starting from (x, y) with the specified scale and color  */
void TextRenderer::RenderText(Shader &shader, string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
	shader.Use();
	glUniformMatrix4fv(shader.ProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(mProjectionMatrix));
	glUniform3f(shader.TextColorLoc, color.x, color.y, color.z);

	// Activate corresponding render state	
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->VAO);

	// Iterate through all characters
	for (unsigned int i = 0; i < text.size(); ++i) {
		const Glyph& ch = this->mCharacters[text[i]];

		GLfloat xpos = x + ch.BearingX * scale;
		GLfloat ypos = y - (ch.Height - ch.BearingY) * scale;

		GLfloat w = ch.Width * scale;
		GLfloat h = ch.Height * scale;

		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Now advance cursors for next glyph
		x += ch.Advance * scale;
	}

	// Unbing VAO/Texture
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}