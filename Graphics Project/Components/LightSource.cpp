#include "LightSource.h"

/* Constructs a light source in a certain position */
LightSource::LightSource(float x, float y, float z) {
	this->Position = glm::vec3(x, y, z);
	this->AmbientColor = glm::vec3(1.0f, 1.0f, 1.0f);
	this->DiffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	this->SpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);
	this->AttenuationConstant = 1.0f;
	this->AttenuationLinear = 0.0f;
	this->AttenuationQuadratic = 0.0f;
}

/* Destructor */
LightSource::~LightSource() {

}

/* Applies light effects by sending the related uniforms to the shader */
void LightSource::ApplyEffects(const Shader& shader) {
	glUniform3f(shader.LightPositionLoc, this->Position.x, this->Position.y, this->Position.z);
	glUniform3f(shader.LightAmbientLoc, this->AmbientColor.x, this->AmbientColor.y, this->AmbientColor.z);
	glUniform3f(shader.LightDiffuseLoc, this->DiffuseColor.x, this->DiffuseColor.y, this->DiffuseColor.z);
	glUniform3f(shader.LightSpecularLoc, this->SpecularColor.x, this->SpecularColor.y, this->SpecularColor.z);
	glUniform1f(shader.LightAttenConstantLoc, this->AttenuationConstant);
	glUniform1f(shader.LightAttenLinearLoc, this->AttenuationLinear);
	glUniform1f(shader.LightAttenQuadraticLoc, this->AttenuationQuadratic);
}