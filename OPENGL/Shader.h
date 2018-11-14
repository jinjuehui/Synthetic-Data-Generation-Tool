#pragma once
#include <GL/glew.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>


class Shader
{

private:
	unsigned int shader_ID;


public:
	Shader(const char* VertexPath, const char* FragmentPath);
	~Shader();

	void use();
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float v0, float v1, float v2, float v3) const;
	void setBool(const std::string &name, bool value) const;

};
