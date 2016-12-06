#pragma once

// STL Includes
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Other Includes
#include "Shader.h"
#include "Texture.h"


/*
	Structure holding vertex attributes
*/
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

/*
	Structure holding material properties
*/
struct Material {
	GLfloat Shininess;
	glm::vec4 AmbientColor;
	glm::vec4 DiffuseColor;
	glm::vec4 SpecularColor;

	/* Constructor with default values */
	Material() {
		this->Shininess = 32.0f;
		this->AmbientColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		this->DiffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		this->SpecularColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
};

/*
	Class used as an abstraction from OpenGL to render meshes easily
*/
class Mesh {
private:
	GLuint VAO, VBO, EBO;
	GLuint mIndicesCount;
	Material mMaterial;
	vector<Texture*> mTextures;

public:
	/* Constructs a mesh from vertices data */
	Mesh(const vector<Vertex>& vertices, const vector<GLuint>& indices, const vector<Texture*> textures, const Material& mtl);

	/* Destructs the mesh */
	~Mesh();

	/* Render the mesh */
	void Draw(const Shader& shader);

private:
	/* Initializes all the buffer objects and arrays from mesh's data */
	void SetupMesh(const vector<Vertex>& vertices, const vector<GLuint>& indices);
};
