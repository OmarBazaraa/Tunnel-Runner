#pragma once
// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Include
#include "Shader.h"


/*
	A class holds light source information used to apply lighting effects
	to the scene by sending values to the related shaders
*/
class LightSource
{
public:
	// Light properties
	glm::vec3 Position;
	glm::vec3 AmbientColor;
	glm::vec3 DiffuseColor;
	glm::vec3 SpecularColor;

	// Light attenuation coefficients
	GLfloat AttenuationConstant;
	GLfloat AttenuationLinear;
	GLfloat AttenuationQuadratic;

	/* Constructs a light source in a certain position */
	LightSource(float x = 0.0f, float y = 5.0f, float z = 0.0f);

	/* Destructor */
	~LightSource();

	/* Applies light effects by sending the related uniforms to the shader */
	void ApplyEffects(const Shader& shader);
};