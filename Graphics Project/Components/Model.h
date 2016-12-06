#pragma once

// STL Includes
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ASSIMP Model Loading Library Includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Other Includes
#include "Mesh.h"


/*
	Class used to load models from given files and render them
	easily abstracted from OpenGL complexity
*/
class Model
{
private:
	// Model Data
	string mDirectory;						// The model directory
	vector<Mesh*> mMeshes;					// Vector of meshes the model consists of
	map<string, Texture*> mLoadedTextures;	// Stores all the textures loaded so far to make sure
											// textures aren't loaded more than once.

public:
	// Model transformation matrix to world coordinates of the scene
	glm::mat4 ModelMatrix;

	/* Constructs a model from the specified file */
	Model(const char* path);

	/* Destructs the model and free resources up */
	~Model();

	/* Draws the model, and thus all its meshes */
	void Draw(const Shader& shader);

private:
	/* Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector */
	void LoadModel(const string& path);

	/* Processes nodes recursively and retrieve their data */
	void ProcessNode(const aiNode* node, const aiScene* scene);

	/* Processes a mesh and retrieve its data */
	Mesh* ProcessMesh(const aiMesh* mesh, const aiScene* scene);

	/* Process the mesh material and load the related textures */
	void ProcessMaterial(const aiMaterial* material, vector<Texture*>& textures, Material& mtl);

	/* Returns all material textures of a given type */
	void LoadMaterialTexture(const aiMaterial* material, aiTextureType type, TextureType myType, vector<Texture*>& textures_list);
};
