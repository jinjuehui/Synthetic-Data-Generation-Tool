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
	vec3 direction;//for Directional Light, for flash light camera.front

	float constantoffset;//for Point Light source, implement attenuation
	float linearfactor;
	float quadraticfactor;

	
	vec3 position;//also for spotlight, for flashlight position=camera.position
	float cutoff;//also for the spotlight, for flashlight 12.5f
	float outercutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform bool SIMPLE_LIGHTNING;

void main()
{

	if (SIMPLE_LIGHTNING) {
	
		vec3 ambient = light.ambient*material.ambient;

		vec3 norm = normalize(Normal);
		vec3 lightDirection = normalize(light.position - fragPosition);

		float diff = max(dot(norm, lightDirection), 0.0f);
		vec3 diffuse = light.diffuse * (diff*material.diffuse);

		vec3 viewDir = normalize(viewPos - fragPosition);
		vec3 reflectLight = reflect(-lightDirection, norm);

		float spec = pow(max(dot(viewDir,reflectLight), 0.0f), material.shininess);
		vec3 specular = light.specular *( spec * material.specular);
		FragColor = vec4(ambient + diffuse + specular, 1);
	}
	else
	{
		vec3 ambient = light.ambient*material.ambient;
		


		vec3 norm = normalize(Normal);
		vec3 lightDirection = normalize(light.position - fragPosition);
					

		//for direction Light:
		//vec3 lightDirection = normalize(-light.direction)

		float diff = max(dot(norm, lightDirection), 0.0f);
		vec3 diffuse = light.diffuse * (diff*material.diffuse);

		vec3 viewDir = normalize(viewPos - fragPosition);
		vec3 reflectLight = reflect(-lightDirection, norm);

		float spec = pow(max(dot(viewDir, reflectLight), 0.0f), material.shininess);
		vec3 specular = light.specular *(spec * material.specular);
		
		float theta = dot(lightDirection, normalize(-light.direction));
		float epsilon = (light.cutoff - light.outercutoff);
		float intensity = clamp((theta - light.outercutoff) / epsilon, 0.0, 1.0);
		diffuse *= intensity;
		specular *= intensity;
		//for point light:
		float distance = length(light.position-fragPosition);
		float attenuation = 1.0/(light.constantoffset+light.linearfactor*distance+light.quadraticfactor*pow(distance,2));
		ambient*=attenuation;
		diffuse*=attenuation;
		specular*=attenuation;
		
		FragColor = vec4(ambient + diffuse + specular, 1);

	}

}