#version 330 core

out vec4 Fragcolor;

uniform vec3 LightColor;

void main()
{
	Fragcolor = vec4(LightColor,1.0f);
}