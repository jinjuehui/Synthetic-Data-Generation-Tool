#version 330 core
layout(location = 0) in vec3 apos;

uniform mat4 model_light;
uniform mat4 projection_light;
uniform mat4 view_light;

void main()
{
	gl_Position = projection_light * view_light * model_light *vec4(apos, 1.0f);	
}