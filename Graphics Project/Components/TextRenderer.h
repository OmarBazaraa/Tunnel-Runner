#pragma once

// STL Includes
#include <iostream>
#include <string>
#include <map>
using namespace std;

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// Other Includes
#include "Shader.h"


/*
	Holds all state information relevant to a character glyph as loaded using FreeType
*/
struct Glyph {
	GLuint TextureID;		// ID handle of the glyph texture
	GLuint Width;			// Width of glyph
	GLuint Height;			// Height of glyph
	GLuint BearingX;		// Offset from baseline to left of glyph
	GLuint BearingY;		// Offset from baseline to top of glyph
	GLuint Advance;			// Horizontal offset to advance to next glyph (in 1/64th pixels)
};

// Constants
const int ASCII_COUNT = 128;


/*
	Class used to render text on OpenGL based applications
*/
class TextRenderer
{
private:
	GLuint VAO, VBO;
	glm::mat4 mProjectionMatrix;
	Glyph mCharacters[ASCII_COUNT];
	
public:
	/* Loads a given font with the specified size */
	TextRenderer(const char* font, int size, float screenWidth, float screenHeight);

	/* Destructs the loaded font */
	~TextRenderer();

	/* Draws the given text starting from (x, y) with the specified scale and color  */
	void RenderText(const Shader &shader, const string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

	/* Returns the width of the given text */
	GLfloat GetTextWidth(const string& text, GLfloat scale) const;
};
