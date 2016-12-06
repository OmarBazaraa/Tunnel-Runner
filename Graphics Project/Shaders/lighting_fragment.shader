#version 330 core

/* Struct holding material properties */
struct Material {
	float shininess;

	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;

	sampler2D ambient_texture1;
	sampler2D ambient_texture2;
	sampler2D ambient_texture3;
	sampler2D diffuse_texture1;
	sampler2D diffuse_texture2;
	sampler2D diffuse_texture3;
	sampler2D specular_texture1;
	sampler2D specular_texture2;
	sampler2D specular_texture3;
};

/* Struct holding light properties */
struct Light {
	vec3 position;
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;

	// Attenuation coefficient
	float atten_constant;
	float atten_linear;
	float atten_quadratic;
};

// Interpolated values from vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Output color from fragment shader
out vec4 color;

// Constant variables for the whole mesh
uniform vec3 camera_position;
uniform Material material;
uniform Light light;

void main() {
	// Ambient
	vec3 ambient = light.ambient_color * material.ambient_color;

	// Diffuse 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = light.diffuse_color * diff * material.diffuse_color;

	// Specular
	vec3 viewDir = normalize(camera_position - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = light.specular_color * spec * material.specular_color;

	// Light attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.atten_constant + light.atten_linear * distance + light.atten_quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// Final color
	color = vec4(ambient + diffuse + specular, 1.0f);
}