
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "stb_image.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime(1.0f), lastFrame(0.0f);

glm::vec3 camera_pose = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse(true);
double lastX(SCR_WIDTH/2), lastY(SCR_HEIGHT/2);
float yaw(-90.0f), pitch(0.0f),fov(45.0f);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}



void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch>89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch<-89.0f)
	{
		pitch = -89.0f;
	}

	glm::vec3 front;
	front.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw))*cos(glm::radians(pitch));
	camera_front = glm::normalize(front);

}


void wasd_keyinput(GLFWwindow* window)
{
	float camera_speed = 0.01f*deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera_pose += camera_speed * camera_front;
	}
	if (glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS)
	{
		camera_pose -= camera_speed * camera_front;
	}
	if (glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS)
	{
		camera_pose -= camera_speed * glm::normalize(glm::cross(camera_front,camera_up));
	}
	if (glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS)
	{
		camera_pose += camera_speed * glm::normalize(glm::cross(camera_front,camera_up));
	}
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

}



const float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};


//draw multiple cubes define location and rotation aches

glm::vec3 CubePosition[] =
{
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};



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
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TEST",NULL,NULL);
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
	//std::cout << "maximum number of vertex attributes supported: " << nrAttributes << std::endl;
	
//1.Generate Vertex Array Object, useful when dealing with multiple vbo layouts
	GLCall(glGenVertexArrays(1,&vao));
	GLCall(glBindVertexArray(vao));
	std::cout << glGetString(GL_VERSION) << std::endl;
	//2.Generate Buffer============================================================================
	VertexBuffer vbo(vertices,sizeof(vertices));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));

	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 5 * sizeof(float),(void*)(sizeof(float)*3)));


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

//5. Rotation and Translation
	/*glm::mat4 projs;
	projs = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT,0.1f,100.0f);*/
	//trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
//create camera coordinate:
	glm::mat4 camera;
	//float radious = 10.0f, camX,camZ;

//create mouse cursor input

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

//4.create shader
	Shader shader_program("VertexShader.shader","FragmentShader.shader");	
	
	shader_program.use();

	shader_program.setInt("texture1", 0);
	shader_program.setInt("texture2", 1);
	
	//shader_program.setMatrix4fv("view", view);


		//glfwSwapInterval(1);
	GLCall(glEnable(GL_DEPTH_TEST));
		//5. while loop	
		while (!glfwWindowShouldClose(window))
		{

			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			GLCall(glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT));


			//GLCall(glDrawArrays(GL_TRIANGLES, 0, 3));
			GLCall(glActiveTexture(GL_TEXTURE0));
			GLCall(glBindTexture(GL_TEXTURE_2D, texture1));
			GLCall(glActiveTexture(GL_TEXTURE1));
			GLCall(glBindTexture(GL_TEXTURE_2D, texture2));
			//std::cout << "System Time: " << (float)glfwGetTime() << std::endl;
			//std::cout << "Size of CubePosition[]: " << sizeof(CubePosition)<< std::endl;
			GLCall(glBindVertexArray(vao));
			//camX = cos(float(glfwGetTime()))*radious;
			//camZ = sin(float(glfwGetTime()))*radious;
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - deltaTime;
			lastFrame = currentFrame;
			wasd_keyinput(window);
			glfwSetCursorPosCallback(window, mouse_callback);
			glfwSetScrollCallback(window, scroll_callback);
			camera = glm::lookAt(camera_pose, camera_pose + camera_front, camera_up);
			shader_program.setMatrix4fv("view", camera);
	
			for (size_t i = 0; i < 10;i++ )
			{
				glm::mat4 trans;
				trans = glm::translate(trans, CubePosition[i]);
				float angle = i * 50;
				trans = glm::rotate(trans,  glm::radians(angle), glm::vec3(0.5f, 0.6f, 0.3f));
				shader_program.setMatrix4fv("transform", trans);
				glm::mat4 projs;
				projs = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
				shader_program.setMatrix4fv("projection", projs);
				//camera = glm::lookAt(glm::vec3{ camX,0,camZ }, glm::vec3{ 0.0,0.0,0.0 }, glm::vec3{ 0.0,1.0,0.0 });
				GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

			}
			//GLCall(glDrawElements(GL_TRIANGLES, 6,GL_UNSIGNED_INT, 0));
			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());

		}
	glfwTerminate();//destroy glcontext
	exit(EXIT_SUCCESS);
	return 0;




}