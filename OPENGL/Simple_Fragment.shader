#version 330 core
out vec4 FragColor;
in vec3 fragPosition;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 LightColor;
uniform vec3 ObjectColor;

vec3 norm = normalize(Normal);
vec3 lightDirection = normalize(lightPos - fragPosition);

float diff = max(dot(norm, lightDirection), 0.0f);
vec3 diffuse = diff * LightColor;

float specularStrength = 0.1;
vec3 viewDir = normalize(viewPos - fragPosition);
vec3 reflectLight = reflect(-lightDirection, norm);

float spec = pow(max(dot(reflectLight, viewDir), 0.0f), 3);
vec3 specular = spec * specularStrength* LightColor;

void main()
{

	float ambientStrength = 0.8f;
	vec3 ambient = ambientStrength * LightColor;
	FragColor = vec4(ambient + diffuse + specular, 1)*vec4(ObjectColor,1.0f);

}