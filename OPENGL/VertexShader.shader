#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 fragPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   TexCoords = aTexCoords;
   gl_Position = projection * view * model * vec4(aPos, 1.0f);
   fragPosition = vec3( model * vec4(aPos, 1.0f));
   Normal = vec3(model * vec4(aNormal, 0.0f));
}