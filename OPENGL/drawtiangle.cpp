#include <GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include"Renderer.h"


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
	"uniform vec4 color;\n"
    "void main()\n"
    "{\n"
    "   FragColor = color;\n"
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//Why it doesn't work with 2,3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//what does this influence
											//if core_profile is chosen, the vertex array object need to be manually created
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
	
	int nrAttributes;
	unsigned int vbo, vao, vertex_shader, fragment_shader, program;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&nrAttributes);
	std::cout << "maximum number of vertex attributes supported: " << nrAttributes << std::endl;
	
// Generate Vertex Array Object, useful when dealing with multiple vbo layouts
	GLCall(glGenVertexArrays(1,&vao));
	GLCall(glBindVertexArray(vao));

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
		GLCall(glAttachShader(program, fragment_shader));
		GLCall(glLinkProgram(program));
		GLCall(glGetProgramiv(program, GL_LINK_STATUS, &success));
		if (!success)
		{
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED";
		}

		int fragment_color;
		fragment_color = glGetUniformLocation(program, "color");
		GLCall(glUseProgram(program));

		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glUseProgram(0));
		GLCall(glBindVertexArray(0));

		//glDeleteShader(vertex_shader);
		//glDeleteShader(fragment_shader);
		float color = 1.0f, r = 0.01f;

		glfwSwapInterval(1);

		//5. while loop	
		while (!glfwWindowShouldClose(window))
		{
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			GLCall(glClear(GL_COLOR_BUFFER_BIT));
			GLCall(glUseProgram(program));
			GLCall(glUniform4f(fragment_color, color, -color, 0.5, 1.0f));

			if (color >= 1 || color <= 0)
				r = -r;

			color += r;
			GLCall(glBindVertexArray(vao))
				GLCall(glDrawArrays(GL_TRIANGLES, 0, 3));
			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());

		}
	glfwTerminate();
	exit(EXIT_SUCCESS);
	return 0;




}