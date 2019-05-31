#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec3 light;

void main()
{
	FragColor = (vec4(0.9,0.9,0.9,1.0)+vec4(light,1.0f))*texture(texture1, TexCoord);
}
