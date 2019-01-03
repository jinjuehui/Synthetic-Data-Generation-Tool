#version 330 core
out vec4 FragColor;
in vec3 fragPosition;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;


vec3 norm = normalize(Normal);
vec3 lightDirection = normalize(lightPos - fragPosition);

float diff = max(dot(norm, lightDirection), 0.0f);
vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);

float specularStrength = 0.9;
vec3 viewDir = normalize(viewPos - fragPosition);
vec3 reflectLight = reflect(-lightDirection, norm);

float spec = pow(max(dot(reflectLight, viewDir), 0.0f), 302);
vec3 specular = spec * specularStrength* vec3(1.0f, 1.0f, 1.0f);

void main()
{

	float ambientStrength = 0.8f;
	vec3 ambient = ambientStrength * vec3(1.0f, 1.0f, 1.0f);
	FragColor = vec4(ambient + diffuse + specular, 1)*vec4(0.6f,0.6f,0.6f,1.0f);

}