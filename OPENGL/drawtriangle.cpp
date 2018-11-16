
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "stb_image.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

}

const float vertices[] =
{
	-0.5f, -0.5f, 0.0f, 1.0f,0.0f,0.0f, 0.0f,0.0f,
	 0.5f, -0.5f, 0.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,
	 0.5f,  0.5f, 0.0f, 0.0f,0.0f,1.0f, 1.0f,1.0f,
	-0.5f,  0.5f, 0.0f, 1.0f,0.0f,0.0f, 0.0f,1.0f
};

const unsigned int index[] =
{
	0,1,2,
	2,0,3
};

//const char* vertex_shader_source = "#version 330 core\n"
//    "layout (location = 0) in vec3 aPos;\n"
//    "void main()\n"
//    "{\n"
//    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//	"}\0";;
//const char* fragment_shader_source = "#version 330 core\n"
//    "out vec4 FragColor;\n"
//	"uniform vec4 color;\n"
//    "void main()\n"
//    "{\n"
//    "   FragColor = color;\n"
//	"}\n\0";;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
 

int main()
{
//0.create window====================================================================
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
		std::cout << "initialize glew failed" << std::endl;
	}
	
	int nrAttributes;
	unsigned int vao, vertex_shader, fragment_shader, program;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&nrAttributes);
	std::cout << "maximum number of vertex attributes supported: " << nrAttributes << std::endl;
	
//1.Generate Vertex Array Object, useful when dealing with multiple vbo layouts
	GLCall(glGenVertexArrays(1,&vao));
	GLCall(glBindVertexArray(vao));

//2.Generate Buffer============================================================================
	VertexBuffer vbo(vertices,sizeof(vertices));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));

	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 8 * sizeof(float),(void*)(sizeof(float)*3)));

	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6)));

//3.create vertex array
	unsigned int eao;
	GLCall(glGenBuffers(1,&eao));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eao));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),index,GL_STATIC_DRAW));

//4.loading texture image and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	unsigned int texture1;
	GLCall(glGenTextures(1, &texture1));

	GLCall(glBindTexture(GL_TEXTURE_2D, texture1));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

	if (data)
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}

	else
	{
		std::cout << "Failed to load the texture!" << std::endl;
	}
	stbi_image_free(data);

	
	unsigned int texture2;
	GLCall(glGenTextures(1, &texture2));
	GLCall(glBindTexture(GL_TEXTURE_2D, texture2));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);

	if (data)
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}

	else
	{
		std::cout << "Failed to load the texture!" << std::endl;
	}
	stbi_image_free(data);

//4.create shader
	Shader shader_program("VertexShader.shader","FragmentShader.shader");
	//GLCall(vertex_shader = glCreateShader(GL_VERTEX_SHADER));
	//GLCall(glShaderSource(vertex_shader,1,&vertex_shader_source,NULL));
	//GLCall(glCompileShader(vertex_shader));
	//int success;
	//char infoLog[512];
	//GLCall(glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success));
	//if (!success)
	//{
	//	glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << std::endl;
	//}

	//GLCall(fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
	//GLCall(glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL));
	//GLCall(glCompileShader(fragment_shader));
	//GLCall(glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success));
	//if (!success)
	//{
	//	glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << std::endl;
	//}

	//
	//	//4.create program
	//	GLCall(program = glCreateProgram());
	//	GLCall(glAttachShader(program, vertex_shader));
	//	GLCall(glAttachShader(program, fragment_shader));
	//	GLCall(glLinkProgram(program));
	//	GLCall(glGetProgramiv(program, GL_LINK_STATUS, &success));
	//	if (!success)
	//	{
	//		glGetProgramInfoLog(program, 512, NULL, infoLog);
	//		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED";
	//	}
	
		
		//fragment_color = glGetUniformLocation(program, "color");
		//GLCall(glUseProgram(program));
		//glDeleteShader(vertex_shader);
		//glDeleteShader(fragment_shader);
		//float color = 1.0f, r = 0.01f;
	
	shader_program.use();
	shader_program.setInt("texture1", 0);
	shader_program.setInt("texture2", 1);

		glfwSwapInterval(1);


		//5. while loop	
		while (!glfwWindowShouldClose(window))
		{
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			GLCall(glClear(GL_COLOR_BUFFER_BIT));


			//GLCall(glUseProgram(program));
			//shader_program.setFloat("color", color, -color, 0.5, 1.0f);
			//GLCall(glUniform4f(fragment_color, color, -color, 0.5, 1.0f));

			/*if (color >= 1 || color <= 0)
				r = -r;
			color += r;*/

			//GLCall(glDrawArrays(GL_TRIANGLES, 0, 3));
			GLCall(glActiveTexture(GL_TEXTURE0));
			GLCall(glBindTexture(GL_TEXTURE_2D, texture1));
			GLCall(glActiveTexture(GL_TEXTURE1));
			GLCall(glBindTexture(GL_TEXTURE_2D, texture2));

			shader_program.use();
			GLCall(glBindVertexArray(vao));
			GLCall(glDrawElements(GL_TRIANGLES, 6,GL_UNSIGNED_INT, 0));
			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());

		}
	glfwTerminate();//destroy glcontext
	exit(EXIT_SUCCESS);
	return 0;




}