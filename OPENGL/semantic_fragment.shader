#version 330 core


uniform vec3 fragcolor;
out vec4 FragColor;


void main()
{
	FragColor = vec4(fragcolor,1.0f);
}
