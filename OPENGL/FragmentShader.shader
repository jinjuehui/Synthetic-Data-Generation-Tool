#version 330 core
out vec4 FragColor;
in vec2 TexCoords;


uniform sampler2D texture_diffuse1;

void main()
{
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * vec3(1.0f,1.0f,1.0f);
	FragColor = vec4(ambient,1)*texture(texture_diffuse1,TexCoords);

}