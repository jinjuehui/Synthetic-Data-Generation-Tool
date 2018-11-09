#include <GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))
static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" <<std::hex<< error << "): " << function <<
			" " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

}

const float vertices[] =
{
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f, 0.5f, 0.0f
};

const char* vertex_shader_source = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";;
const char* fragment_shader_source = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n\0";;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
 

int main()
{
//1.create window====================================================================
	GLFWwindow* window;

	if (!glfwInit())
		exit(EXIT_FAILURE);
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);//Why it doesn't work with 2,3
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//what does this influence

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TRIANGLE",NULL,NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);

	if (glewInit()!=GLEW_OK)
	{
		std::cout << "initialze glew failed" << std::endl;
	}
	

	unsigned int vbo, vertex_shader, fragment_shader, program;

//2.Generate Buffer============================================================================
	GLCall(glGenBuffers(1, &vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));


//3.create shader
	GLCall(vertex_shader = glCreateShader(GL_VERTEX_SHADER));
	GLCall(glShaderSource(vertex_shader,1,&vertex_shader_source,NULL));
	GLCall(glCompileShader(vertex_shader));
	int success;
	char infoLog[512];
	GLCall(glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success));
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << std::endl;
	}

	GLCall(fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
	GLCall(glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL));
	GLCall(glCompileShader(fragment_shader));
	GLCall(glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success));
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << std::endl;
	}


//4.create program
	GLCall(program = glCreateProgram());
	GLCall(glAttachShader(program, vertex_shader));
	GLCall(glAttachShader(program,fragment_shader));
	GLCall(glLinkProgram(program));
	GLCall(glGetProgramiv(program,GL_LINK_STATUS,&success));
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED";
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	
//5. while loop	
	while (!glfwWindowShouldClose(window))
	{
		GLCall(glClearColor(0.2f,0.3f,0.3f,1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		GLCall(glUseProgram(program));
		GLCall(glDrawArrays(GL_TRIANGLES,0,3));
		GLCall(glfwSwapBuffers(window));
		GLCall(glfwPollEvents());

	}
	glfwTerminate();
	exit(EXIT_SUCCESS);
	return 0;




}