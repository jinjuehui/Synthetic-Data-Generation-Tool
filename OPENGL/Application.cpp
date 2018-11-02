#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};



static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos) //std::string::npos  invalid string position
		{
			if (line.find("vertex") != std::string::npos)
			{// set mode to vertex
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{//set mode to fragment
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return{ ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type , const std::string& source)
{
	unsigned int id =  glCreateShader(type); //why it color not changed when GL_VERTEX_SHADER is given
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);


	// error handeling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message =(char*) alloca(length*sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << message<< "Failed to compile"
				<<(type == GL_VERTEX_SHADER ? "vertex" : "fragment")<<"shader!"<< std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}


	return id;
}



static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);
	////
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;

}


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;



	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Triangle", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	
	float positions[8] = {
		-0.5f,-0.5f,
		 0.5f, 0.5f,
		 0.5f, -0.5f,
		 -0.5f, 0.5f,
	};

	unsigned int indices[] =
	{
		0,1,2,
		0,1,3
	};


	unsigned int buffer;

	glGenBuffers(1, &buffer);
	//choose the buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//specify the data
	glBufferData(GL_ARRAY_BUFFER,12*sizeof(float), positions, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);


	unsigned int ibo; //indices buffer object
	glGenBuffers(1, &ibo);
	//choose the buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	//specify the data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);


	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	ShaderProgramSource source = ParseShader("Basic.shader");
	std::cout << "VERTEX" << std::endl;
	std::cout << source.VertexSource << std::endl;
	std::cout << "FRAGMENT" << std::endl;
	std::cout << source.FragmentSource << std::endl;


	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		
		//glDrawArrays(GL_TRIANGLES, 0,6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,nullptr);
		/* TEST DRAW A TRIANGLE
		glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f,-0.5f);
		glVertex2f(0.0f, 0.5f);
		glVertex2f(0.0f, -0.5f);
		glEnd();
		*/
		

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}