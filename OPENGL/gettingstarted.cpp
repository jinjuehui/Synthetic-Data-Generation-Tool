#include <GL/glew.h>	
#include <GLFW/glfw3.h>

#include "linmath.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(error_callback(#x,__FILE__, __LINE__))


static const struct
{
	float x, y;
	float r, g, b;
}

vertices[3] = 
{
	{-0.6f,-0.4f,1.0f,0.f,0.f},
	{0.6f,-0.4f,0.f,1.f,0.f},
	{0.f,0.6f,0.f,0.f,1.f}

};


static const char* vertex_shader_text =

"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text = 
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";


static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}


static bool error_callback(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ": " << line << std::endl;
		return false;
	}
	return true;
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s \n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window,GLFW_TRUE);
}

int main(void)
{
	GLFWwindow* window;
	unsigned int vertex_buffer, vertex_shader, fragment_shader, program;
	int mvp_location, vpos_location, vcol_location;

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(640, 480, "Simple Example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	
	if (glewInit() != GLEW_OK)
		std::cout << "Error" << std::endl;

	std::cout << glGetString(GL_VERSION);

	glfwSwapInterval(1);

	GLCall(glGenBuffers(1, &vertex_buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices, GL_STATIC_DRAW));

	GLCall(vertex_shader = glCreateShader(GL_VERTEX_SHADER));
	GLCall(glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL));
	GLCall(glCompileShader(vertex_shader));

	GLCall(fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
	GLCall(glShaderSource(fragment_shader,1,&fragment_shader_text,NULL));
	GLCall(glCompileShader(fragment_shader));

	GLCall(program = glCreateProgram());
	GLCall(glAttachShader(program, vertex_shader));
	GLCall(glAttachShader(program, fragment_shader));
	GLCall(glLinkProgram(program));

	GLCall(mvp_location = glGetUniformLocation(program, "MVP"));
	GLCall(vpos_location = glGetAttribLocation(program, "vPos"));
	GLCall(vcol_location = glGetAttribLocation(program, "vCol"));

	GLCall(glEnableVertexAttribArray(vpos_location));
	GLCall(glVertexAttribPointer(vpos_location, 2, GL_FLOAT,GL_FALSE,sizeof(float)*5,(void*)(sizeof(float)*2)));

	GLCall(glEnableVertexAttribArray(vcol_location));
	GLCall(glVertexAttribPointer(vcol_location, 3, GL_FLOAT,GL_FALSE,sizeof(float)*5,(void*)(sizeof(float)*2)));

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		mat4x4 m, p, mvp;

		GLCall(glfwGetFramebufferSize(window, &width, &height));
		ratio = width / (float)height;

		GLCall(glViewport(0, 0, width, height));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		mat4x4_identity(m);
		mat4x4_rotate_Z(m,m,(float) glfwGetTime());
		mat4x4_ortho(p,-ratio,ratio,-1.f,1.f,1.f,-1.f);
		mat4x4_mul(mvp, p, m);

		GLCall(glUseProgram(program));
		GLCall(glUniformMatrix4fv(mvp_location, 1, GL_FALSE,(const GLfloat*) mvp));
		GLCall(glDrawArrays(GL_TRIANGLES,0,3));

		GLCall(glfwSwapBuffers(window));
		GLCall(glfwPollEvents());

	}


	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
	return 0;

}