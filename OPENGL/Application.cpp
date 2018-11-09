#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>


#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}


static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): " << function << 
			" "<< file <<":" << line <<std::endl;
		return false;
	}
	return true;
}


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
			{
				type = ShaderType::VERTEX;// set mode to vertex			
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;//set mode to fragment
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
	glDeleteShader(vs);//once linked the shader object won't be needed anymore
	glDeleteShader(fs);

	return program;

}


int main(void)
{
	GLFWwindow* window;
	
	if (!glfwInit())	// Initialize the library 
		return -1;

	//framerate setting
	//glfwSwapInterval(1);

	window = glfwCreateWindow(640, 480, "Triangle", NULL, NULL);	// Create a windowed mode window and its OpenGL context 
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);	// Make the window's context current 

	if (glewInit() != GLEW_OK)  //initialize the extension loader 
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
	

	GLCall(glGenBuffers(1, &buffer));//choose the buffer
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));	//specify the data
	GLCall(glBufferData(GL_ARRAY_BUFFER,12*sizeof(float), positions, GL_STATIC_DRAW));
	
	GLCall(glEnableVertexAttribArray(0));//0:vertex attribute location
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));
	//tell OpenGL how it should interpret the vertex data(per vertex attribute) 
	//first parameter: which attribute we want to configure, the location of the vertex attribute
	//second: the size of the vertex attribute
	//third: the type of the data 
	//fourth: if we want the data tobe normalized
	//fifth: stride, the space between consecutive vertex attributes
	//sixth: type void*. this is the offset of where the position data begins in the buffer

	unsigned int ibo; 
	GLCall(glGenBuffers(1, &ibo));	//indices buffer object
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));//choose the buffer
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));	//specify the data

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));


	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	ShaderProgramSource source = ParseShader("Basic.shader");
	//std::cout << "VERTEX" << std::endl;
	//std::cout << source.VertexSource << std::endl;
	//std::cout << "FRAGMENT" << std::endl;
	//std::cout << source.FragmentSource << std::endl;
	

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glUseProgram(shader));
	
	GLCall(int location = glGetUniformLocation(shader, "u_Color"));
	ASSERT(location != -1);
	GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

	float r = 0.0f;
	float increment = 0.05f;
	while (!glfwWindowShouldClose(window)) //Loop until the user closes the window 
	{
		
		GLCall(glfwSetKeyCallback(window, key_callback)); //Render here
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		
		//GLClearError();
		//glDrawArrays(GL_TRIANGLES, 0,6);
		GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,nullptr));
		//ASSERT(GLLogCall());


		if (r > 1.0f)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = 0.05f;

		r += increment;

		// TEST DRAW A TRIANGLE
		//glBegin(GL_TRIANGLES);
		//glVertex2f(-0.5f,-0.5f);
		//glVertex2f(0.0f, 0.5f);
		//glVertex2f(0.0f, -0.5f);
		//glEnd();
		//
		

		
		GLCall(glfwSwapBuffers(window));// Swap front and back buffers 

		
		GLCall(glfwPollEvents());//Poll for and process events 
	}

	GLCall(glDeleteProgram(shader));

	GLCall(glfwTerminate());
	return 0;
}  

