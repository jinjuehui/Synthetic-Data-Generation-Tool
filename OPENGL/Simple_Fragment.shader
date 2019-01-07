#version 330 core
out vec4 FragColor;
in vec3 fragPosition;
in vec3 Normal;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{

	vec3 ambient = light.ambient*material.ambient;

	vec3 norm = normalize(Normal);
	vec3 lightDirection = normalize(light.position - fragPosition);

	float diff = max(dot(norm, lightDirection), 0.0f);
	vec3 diffuse = light.diffuse * (diff*material.diffuse);

	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - fragPosition);
	vec3 reflectLight = reflect(-lightDirection, norm);

	float spec = pow(max(dot(viewDir,reflectLight), 0.0f), material.shininess);
	vec3 specular = light.specular *( spec * material.specular);
	FragColor = vec4(ambient + diffuse + specular, 1);

}