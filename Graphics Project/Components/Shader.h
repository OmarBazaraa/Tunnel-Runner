#pragma once

// STL Includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

// GL Includes
#include <GL/glew.h>

// Attributes and uniform constants
#define VERTEX_POSITION_LOC				0
#define VERTEX_NORMAL_LOC				1
#define VERTEX_TEXTURE_COORD_LOC		2
#define MODEL_MATRIX_LOC				"model"
#define VIEW_MATRIX_LOC					"view"
#define PROJECTION_MATRIX_LOC			"projection"
#define CAMERA_POSITION_LOC				"camera_position"
#define MATERIAL_SHININESS_LOC			"material.shininess"
#define MATERIAL_AMBIENT_COLOR_LOC		"material.ambient_color"
#define MATERIAL_DIFFUSE_COLOR_LOC		"material.diffuse_color"
#define MATERIAL_SPECULAR_COLOR_LOC		"material.specular_color"
#define MATERIAL_AMBIENT_TEXTURE_LOC	"material.ambient_texture"
#define MATERIAL_DIFFUSE_TEXTURE_LOC	"material.diffuse_texture"
#define MATERIAL_SPECULAR_TEXTURE_LOC	"material.specular_texture"
#define LIGHT_POSITION_LOC				"light.position"
#define LIGHT_AMBIENT_LOC				"light.ambient_color"
#define LIGHT_DIFFUSE_LOC				"light.diffuse_color"
#define LIGHT_SPECULAR_LOC				"light.specular_color"
#define LIGHT_ATTEN_CONSTANT_LOC		"light.atten_constant"
#define LIGHT_ATTEN_LINEAR_LOC			"light.atten_linear"
#define LIGHT_ATTEN_QUADRATIC_LOC		"light.atten_quadratic"
#define TEXT_SAMPLER_LOC				"text"
#define TEXT_COLOR_LOC					"text_color"

/*
	A shader program class which compiles vertex and fragment shaders
	and link them into a single program used for scene rendering
*/
class Shader
{
public:
	// Shader program id
	GLuint ProgramID;

	// Vertex Attributes
	GLint VertexPositionLoc;
	GLint VertexNormalLoc;
	GLint VertexTextureCoordLoc;

	// Matrices
	GLint ModelMatrixLoc;
	GLint ViewMatrixLoc;
	GLint ProjectionMatrixLoc;

	// Camera
	GLint CameraPositionLoc;

	// Material
	GLint MaterialShininessLoc;
	GLint MaterialAmbientColorLoc;
	GLint MaterialDiffuseColorLoc;
	GLint MaterialSpecularColorLoc;
	GLint MaterialAmbientTextureLoc;
	GLint MaterialDiffuseTextureLoc;
	GLint MaterialSpecularTextureLoc;
	
	// Light
	GLint LightPositionLoc;
	GLint LightAmbientLoc;
	GLint LightDiffuseLoc;
	GLint LightSpecularLoc;
	GLint LightAttenConstantLoc;
	GLint LightAttenLinearLoc;
	GLint LightAttenQuadraticLoc;

	// Text
	GLint TextSamplerLoc;
	GLint TextColorLoc;

	/* Compiles and links the vertex and fragment shaders into a program */
	Shader(const char* vertex_path, const char* fragment_path);

	/* Destructs the compiled program when it is out of scope */
	~Shader();

	/* Activates the current shader */
	void Use() const;

private:
	/* Setup shader's attribute and uniform locations */
	void SetupLocations();
};
