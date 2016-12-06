#include "Mesh.h"

/* Constructs a mesh from vertices data */
Mesh::Mesh(const vector<Vertex>& vertices, const vector<GLuint>& indices, const vector<Texture*> textures, const Material& mtl) {
	this->mIndicesCount = indices.size();
	this->mTextures = textures;
	this->mMaterial = mtl;

	this->VAO = -1;
	this->VBO = -1;
	this->EBO = -1;

	this->SetupMesh(vertices, indices);
}

/* Destructs the mesh */
Mesh::~Mesh() {
	// Release mesh's data from the memory
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
	glDeleteVertexArrays(1, &this->VAO);

	this->VAO = -1;
	this->VBO = -1;
	this->EBO = -1;

	// Release textures data
	/*for (unsigned int i = 0; i < this->mTextures.size(); ++i) {
		if (this->mTextures[i] != NULL) {
			delete this->mTextures[i];
			this->mTextures[i] = NULL;
		}
	}*/
}

/* Render the mesh */
void Mesh::Draw(const Shader& shader) {
	// Send material properties to the related shader
	glUniform1f(shader.MaterialShininessLoc, this->mMaterial.Shininess);
	glUniform3f(shader.MaterialAmbientColorLoc, this->mMaterial.AmbientColor.x, this->mMaterial.AmbientColor.y, this->mMaterial.AmbientColor.z);
	glUniform3f(shader.MaterialDiffuseColorLoc, this->mMaterial.DiffuseColor.x, this->mMaterial.DiffuseColor.y, this->mMaterial.DiffuseColor.z);
	glUniform3f(shader.MaterialSpecularColorLoc, this->mMaterial.SpecularColor.x, this->mMaterial.SpecularColor.y, this->mMaterial.SpecularColor.z);

	// Bind appropriate textures
	GLuint ambientCount = 0;
	GLuint diffuseCount = 0;
	GLuint specularCount = 0;
	
	for (unsigned int i = 0; i < this->mTextures.size(); ++i) {
		// Active proper texture unit before binding
		glActiveTexture(GL_TEXTURE0 + i);

		// Retrieve texture number
		stringstream ss;

		switch (this->mTextures[i]->Type)
		{
		case TEXTURE_AMBIENT:
			ss << MATERIAL_AMBIENT_TEXTURE_LOC << ++ambientCount;
			break;
		case TEXTURE_DIFFUSE:
			ss << MATERIAL_DIFFUSE_TEXTURE_LOC << ++diffuseCount;
			break;
		case TEXTURE_SPECULAR:
			ss << MATERIAL_SPECULAR_TEXTURE_LOC << ++specularCount;
			break;
		default:
			break;
		}

		// Now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader.ProgramID, ss.str().c_str()), i);

		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, this->mTextures[i]->ID);
	}

	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->mIndicesCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Always good practice to set everything back to defaults once configured.
	for (unsigned int i = 0; i < this->mTextures.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

/* Initializes all the buffer objects and arrays from mesh's data */
void Mesh::SetupMesh(const vector<Vertex>& vertices, const vector<GLuint>& indices) {
	// Create buffers/arrays
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	// Bind the vertex array
	glBindVertexArray(this->VAO);

	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	
	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(VERTEX_POSITION_LOC);
	glVertexAttribPointer(VERTEX_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::Position));
	// Vertex Normals
	glEnableVertexAttribArray(VERTEX_NORMAL_LOC);
	glVertexAttribPointer(VERTEX_NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::Normal));
	// Vertex Texture Coordinates
	glEnableVertexAttribArray(VERTEX_TEXTURE_COORD_LOC);
	glVertexAttribPointer(VERTEX_TEXTURE_COORD_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::TexCoords));

	// Unbind vertex array and buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Disable attributes
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
