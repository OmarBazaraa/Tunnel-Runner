#include "Model.h"

/* Constructs a model from the specified file */
Model::Model(const char* path) {
	this->LoadModel(path);

	this->ModelMatrix = glm::mat4(1.0f); // Identity matrix
}

/* Destructs the model and free resources up */
Model::~Model() {
	// Release meshes data
	for (unsigned int i = 0; i < this->mMeshes.size(); ++i) {
		if (this->mMeshes[i] != NULL) {
			delete this->mMeshes[i];
			this->mMeshes[i] = NULL;
		}
	}

	// Release model textures
	while (!this->mLoadedTextures.empty()) {
		delete this->mLoadedTextures.begin()->second;
		this->mLoadedTextures.erase(this->mLoadedTextures.begin());
	}
}

/* Draws the model, and thus all its meshes */
void Model::Draw(const Shader& shader) {
	glUniformMatrix4fv(shader.ModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(this->ModelMatrix));

	for (unsigned int i = 0; i < this->mMeshes.size(); i++) {
		this->mMeshes[i]->Draw(shader);
	}
}

/* Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector */
void Model::LoadModel(const string& path) {
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	// Check for errors
	if (scene == NULL || scene->mRootNode == NULL || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) {
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	// Retrieve the directory path of the file path
	this->mDirectory = path.substr(0, path.find_last_of('/'));

	// Process ASSIMP's root node recursively
	this->ProcessNode(scene->mRootNode, scene);
}

/* Processes nodes recursively and retrieve their data */
void Model::ProcessNode(const aiNode* node, const aiScene* scene) {
	// Process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->mMeshes.push_back(this->ProcessMesh(mesh, scene));
	}

	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		this->ProcessNode(node->mChildren[i], scene);
	}
}

/* Processes a mesh and retrieve its data */
Mesh* Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene) {
	// Mesh data to fill
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture*> textures;
	Material mtl;

	// Loop through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		glm::vec3 vector;

		// Position
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// Normal
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		// Texture Coordinates
		if (mesh->mTextureCoords[0] != NULL) {
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		
		vertices.push_back(vertex);
	}

	// Loop through each of the mesh's faces and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		const aiFace& face = mesh->mFaces[i];

		// Retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process materials
	if (mesh->mMaterialIndex >= 0) {
		const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		this->ProcessMaterial(material, textures, mtl);
	}

	// Return a mesh object created from the extracted mesh data
	return new Mesh(vertices, indices, textures, mtl);
}

/* Process the mesh material and load the related textures */
void Model::ProcessMaterial(const aiMaterial* material, vector<Texture*>& textures, Material& mtl) {
	// Extract material shininess
	material->Get(AI_MATKEY_SHININESS, mtl.Shininess);

	// Extract material color
	aiColor4D color;
	// Ambient color
	if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
		mtl.AmbientColor = glm::vec4(color.r, color.g, color.b, color.a);
	}
	// Diffuse color
	if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
		mtl.DiffuseColor = glm::vec4(color.r, color.g, color.b, color.a);
	}
	// Specular color
	if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
		mtl.SpecularColor = glm::vec4(color.r, color.g, color.b, color.a);
	}	

	// Load textures
	this->LoadMaterialTexture(material, aiTextureType_AMBIENT, TEXTURE_AMBIENT, textures);
	this->LoadMaterialTexture(material, aiTextureType_DIFFUSE, TEXTURE_DIFFUSE, textures);
	this->LoadMaterialTexture(material, aiTextureType_SPECULAR, TEXTURE_SPECULAR, textures);
}

/* Returns all material textures of a given type */
void Model::LoadMaterialTexture(const aiMaterial* material, aiTextureType type, TextureType myType, vector<Texture*>& textures_list) {
	unsigned int TexturesCount = material->GetTextureCount(type);

	// Loop through each of the material's texture s
	for (unsigned int i = 0; i < TexturesCount; ++i) {
		aiString str;

		if (material->GetTexture(type, i, &str) != AI_SUCCESS) {
			std::cout << "ERROR::ASSIMP::TEXTURE_NOT_SUCCESFULLY_READ\n" << str.C_Str() << std::endl;
			continue;
		}

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		map<string, Texture*>::iterator it = this->mLoadedTextures.find(str.C_Str());

		if (it == this->mLoadedTextures.end()) {
			// Texture needs to be loaded for the first time
			string path = this->mDirectory + "/" + str.C_Str();
			Texture* texture = new Texture(path.c_str(), myType);
			textures_list.push_back(texture);
			this->mLoadedTextures[str.C_Str()] = texture;  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
		else {
			// Texture was loaded before
			textures_list.push_back(it->second);
		}
	}
}
