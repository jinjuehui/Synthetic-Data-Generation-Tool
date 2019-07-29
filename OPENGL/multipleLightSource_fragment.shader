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

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


struct PointLight {
	vec3 position;

	float constantoffset;//for Point Light source, implement attenuation
	float linearfactor;
	float quadraticfactor;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutoff;
	float outercutoff;

	float constantoffset;
	float linearfactor;
	float quadraticfactor;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


uniform vec3 viewPos;
uniform Material material;


#define MAX_LIGHT_NUMBER 10
uniform int point_light_num;

uniform DirLight directionlight;
uniform PointLight pointlights[MAX_LIGHT_NUMBER];
uniform SpotLight spotlight;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragpos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragpos, vec3 viewDir);


void main()
{

	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - fragPosition);
	vec3 result = CalcDirLight(directionlight, norm, viewDir);

	for (int i=0; i<point_light_num; i++)
	{
		result += CalcPointLight(pointlights[i], norm, fragPosition, viewDir);
	}

	//result += CalcSpotLight(spotlight, norm, fragPosition, viewDir);
	
	FragColor = vec4(result, 1.0);


}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0f);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

	vec3 ambient = light.ambient*material.ambient;
	vec3 diffuse = light.diffuse*diff*material.diffuse;
	vec3 specular = light.specular*spec*material.specular;

	return (ambient + diffuse + specular);
	
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragpos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragpos);

	float diff = max(dot(normal, lightDir), 0.0);
	
	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float distance = length(light.position-fragpos);
	float attenuation = 1.0 / (light.constantoffset + light.linearfactor*distance + light.quadraticfactor * distance*distance);


	vec3 ambient = light.ambient*material.ambient;
	vec3 diffuse = light.diffuse*diff*material.diffuse;
	vec3 specular = light.specular*spec*material.specular;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);

}
	
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragpos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position-fragpos);

	float diff = max(dot(normal,lightDir), 0.0f);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float distance = length(light.position-fragpos);
	float attenuation = 1.0 / (light.constantoffset + light.linearfactor*distance + light.quadraticfactor * pow(distance, 2));

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutoff - light.outercutoff;
	float intensity = clamp((theta-light.outercutoff)/epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient*material.ambient;
	vec3 diffuse = light.diffuse*diff*material.diffuse;
	vec3 specular = light.specular*spec*material.specular;

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}