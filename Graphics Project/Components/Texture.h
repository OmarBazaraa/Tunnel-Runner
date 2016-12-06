#pragma once

// STL Includes
#include <string>
using namespace std;

// GL Includes
#include <GL/glew.h>

// Image Loading Library Includes
#include <SOIL/SOIL.h>


/*
	Defines several types for textures
*/
enum TextureType {
	TEXTURE_AMBIENT,
	TEXTURE_DIFFUSE,
	TEXTURE_SPECULAR,
	TEXTURE_HEIGHT,
	TEXTURE_UNKNOWN
};

/*
	Class used to load textures from given files
*/
class Texture
{
public:
	GLuint ID;
	TextureType Type;

	/* Loads texture from a file */
	Texture(const char* path, TextureType type);

	/* Destructs the texture and free resources up */
	~Texture();
};
