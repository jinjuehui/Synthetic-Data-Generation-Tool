#include "Renderer.h"
#include "Shader.h"


Shader::Shader(const char* VertexPath, const char* FragmentPath)
{
	std::string VertexCode;
	std::string FragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit || std::ifstream::badbit);
	vShaderFile.exceptions(std::ifstream::failbit || std::ifstream::badbit);

	try
	{
		vShaderFile.open(VertexPath);
		fShaderFile.open(FragmentPath);
		
		std::stringstream vShaderStream, fShaderStream;
		
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		VertexCode = vShaderStream.str();
		FragmentCode = fShaderStream.str();

	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FAILED_NOT_SUCCESFULLY_READ" << std::endl;
	}
	
	const char* vShaderCode = VertexCode.c_str();
	const char* fShaderCode = FragmentCode.c_str();


	//Compile and Link Shader program

	unsigned int vertex, fragment;

	GLCall(vertex = glCreateShader(GL_VERTEX_SHADER));
	GLCall(fragment = glCreateShader(GL_FRAGMENT_SHADER));

	GLCall(glShaderSource(vertex, 1, &vShaderCode, NULL));
	GLCall(glShaderSource(fragment, 1, &fShaderCode, NULL));

	GLCall(glCompileShader(vertex));
	GLCall(glCompileShader(fragment));

	int vsuccess, fsuccess;
	char infoLog[512];

	GLCall(glGetShaderiv(vertex, GL_COMPILE_STATUS, &vsuccess));
	GLCall(glGetShaderiv(fragment, GL_COMPILE_STATUS, &fsuccess));
	
	if (!vsuccess)
	{
		GLCall(glGetShaderInfoLog(vertex,512,NULL,infoLog));
		std::cout << "ERROR::VERTEX_SHADER::COMPILE::FAILED " << infoLog	<< std::endl;
	}
	
	if (!fsuccess)
	{
		GLCall(glGetShaderInfoLog(fragment, 512, NULL, infoLog));
		std::cout << "ERROR::FRAGMENT_SHADER::COMPILE::FAILED " <<infoLog<< std::endl;
	}

	GLCall(shader_ID = glCreateProgram());
	GLCall(glAttachShader(shader_ID,vertex));
	GLCall(glAttachShader(shader_ID,fragment));
	GLCall(glLinkProgram(shader_ID));
	int program_success;
	GLCall(glGetProgramiv(shader_ID,GL_LINK_STATUS,&program_success));

	if (!program_success)
	{
		GLCall(glGetProgramInfoLog(shader_ID, 512, NULL, infoLog));
		std::cout << "ERROR::LINK_PROGRAM::FAILED " << infoLog << std::endl;
	}

	GLCall(glDeleteShader(vertex));
	GLCall(glDeleteShader(fragment));
}
	
Shader::~Shader()
{
	GLCall(glUseProgram(0));
}

void Shader::use()
{
	GLCall(glUseProgram(shader_ID));
}

//not quite understand why uniform1*() not uniform4*()
void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(shader_ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(shader_ID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float v0, float v1, float v2, float v3) const
{
	glUniform4f(glGetUniformLocation(shader_ID, name.c_str()),v0,v1,v2,v3);
}

void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(shader_ID, name.c_str()), value);
}

void Shader::setMatrix4fv(const std::string &name, glm::mat4 value) 
{
	glUniformMatrix4fv(glGetUniformLocation(shader_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	//std::cout << "in setMatrix4fv()"<< glm::value_ptr(value) << std::endl;
}


void Shader::setVector3f(const std::string &name, glm::vec3 value)
{
	glUniform3f(glGetUniformLocation(shader_ID, name.c_str()),value[0],value[1],value[2]);
	//std::cout << "in setMatrix4fv()"<< glm::value_ptr(value) << std::endl;
}