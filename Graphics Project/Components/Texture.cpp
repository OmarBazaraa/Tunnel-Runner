#include "Texture.h"

/* Loads texture from a file */
Texture::Texture(const char* path, TextureType type) {
	// Set texture type needed to detect the uniform name in shader
	this->Type = type;

	// Load image from file
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);

	// Copy image data to the bound texture
	glGenTextures(1, &this->ID);
	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Release image data
	SOIL_free_image_data(image);
}

/* Destructs the texture and free resources up */
Texture::~Texture() {
	glDeleteTextures(1, &this->ID);
	this->ID = -1;
}